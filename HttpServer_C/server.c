#include "http.h"

void CGI(int,char*,char* , Header);
void handle_connect(int, struct sockaddr_in *);

void CGI(int new_sockFD, char* cgi_type, char* file_name, Header header) {
	// Create two pipe used to connect with CGI
	int cgiSTDIN[2];	// Will used to dup stdin
    int cgiSTDOUT[2];	// Will used to dup stdout
	if(pipe(cgiSTDIN)<0){
        perror("Pipe Error: ");
        exit(EXIT_FAILURE);
    }
    if(pipe(cgiSTDOUT)<0){
        perror("Pipe Error: ");
        exit(EXIT_FAILURE);
    }

	pid_t pid;
	// Fork a child to handle CGI
	if((pid = fork()) < 0){
		perror("Fork Error at CGI: ");
		exit(EXIT_FAILURE);
	}
	else{
		// Child process
		if(pid == 0){
			// Close unused pipe
			close(cgiSTDIN[1]);
			close(cgiSTDOUT[0]);

			// Redirect STDOUT of CGI to cgiSTDIN
			dup2(cgiSTDOUT[1],STDOUT_FILENO);
			// Redirect STDIN of CGI to cgiSTDOUT
			dup2(cgiSTDIN[0],STDIN_FILENO);

			// Close old fd after redirect
			close(cgiSTDIN[0]);
			close(cgiSTDOUT[1]);

			// Execute the CGI
			execlp(cgi_type, cgi_type, NULL);
			exit(EXIT_SUCCESS);
		}
		// Parent process
		else{
			// Close unused pipe
			close(cgiSTDIN[0]);
			close(cgiSTDOUT[1]);

			char c;
			int status;
			// Send filename to CGI
			write(cgiSTDIN[1], file_name,strlen(file_name));
			
			if (strcmp(cgi_type, INSERT_CGI) == 0){
				// Send the query
				write(cgiSTDIN[1],"\n",1);
				write(cgiSTDIN[1],header.Query,strlen(header.Query));
			}

			// receive the message from the  CGI program
			while (read(cgiSTDOUT[0], &c, 1) > 0){
				// output the message to terminal
				write(STDOUT_FILENO, &c, 1);
				// Send to socket
				write(new_sockFD, &c, 1);
			}
			// output the message to terminal, and send to socket
			send(STDOUT_FILENO, "\n", 1, 0);
			send(new_sockFD,"\n",1,0);

            // connection finish
            close(cgiSTDOUT[0]);
            close(cgiSTDIN[1]);
            waitpid(pid, &status, 0);
		}
	}

}

void handle_connect(int new_sockFD, struct sockaddr_in *client_addr){
	
	char request[BUFF_SIZE] = {0};	// Http request
	char resource[BUFF_SIZE] = {0}; // Resource in server

	// Recieve the Client request
	read(new_sockFD, request, BUFF_SIZE);
	
	DEBUG("%s \n",request);
	
	// Parsing the request header
	Header header = request_header(request);
	printf("Header Method: %s, Url: %s, Protocol: %s\n",header.Method,header.Url,header.Protocol);
	printf("Header CL: %s\n",header.Content_Length);
	printf("Header CT: %s\n",header.Content_Type);
	printf("Header QUERY: %s\n",header.Query);
	printf("Get request from %s:%d \"%s %s %s\"\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), header.Method,header.Url,header.Protocol);

	// Check if HTTP request
	if((strcmp(header.Protocol,"HTTP/1.1")) != 0){
		perror("Not Http/1.1: ");
		exit(EXIT_FAILURE);
	}
	else{
		// Get: method = 1, POST, method = 2
		int method = -1;
		if(strcmp(header.Method,"GET") == 0)
			method = 1;
		else if(strcmp(header.Method, "POST") == 0)
			method = 2;

		// Only handle the GET request or POST request
		if(method == -1){
			printf("Unknown Request!!\n");
		}
		else{
			// When url end with '/', set it as "/DEFAULT_PAGE"
			if(header.Url[strlen(header.Url)-1] == '/'){
				strcat(header.Url, DEFAULT_PAGE);
			}

			// The Url resource
			strcpy(resource,ROOT);
			strcat(resource,header.Url);	// e.g. "ROOT/xxx/DEFAULT_PAGE"
			
			DEBUG("Resource: %s\n",resource);
			
			// Open the resource file
			int fd = open(resource,O_RDONLY);

			// File not found, cause 404
			if (fd == -1){
				printf("404 Not Found\n");
				// Send 404 Header
				response_header(new_sockFD,STATUS_404);

				// Open the 404.html
				memset(resource, 0, BUFF_SIZE);
				strcpy(resource,ROOT);
				strcat(resource,"/404.html");
				
				// Send the 404.html
				CGI(new_sockFD, VIEW_CGI, resource, header);
			}
			else{
				// Send 200 Header
				response_header(new_sockFD,STATUS_200);

				// Handle GET request
				if(method == 1)
					CGI(new_sockFD, VIEW_CGI, resource, header);
				// Handle POST request
				else if(method == 2){
					CGI(new_sockFD, INSERT_CGI, resource, header);
				}
			}
			close(fd);
		}
	}
}

int main(int argc,char const *argv[]){
	
	// Parsing the cmd input
	Parsing_CMD(argc, argv);
	
	int sockFD, new_sockFD;
	struct sockaddr_in server_addr, client_addr;
	int cli_addr_len = sizeof(client_addr);
	
	// Create new socket
	if((sockFD = socket(AF_INET,SOCK_STREAM,0))==0){
		perror("Socket Error:");
		exit(EXIT_FAILURE);
	}

	// Set Socket to be reusable address
	int yes = 1;
	if(setsockopt(sockFD,SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		perror("Setting socket option to SO_REUSEADDR");
		exit(EXIT_FAILURE);
	}

	// Binding
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	memset(server_addr.sin_zero, '\0',sizeof(server_addr.sin_zero));
	
	if(bind(sockFD,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}
	// Ready to accept new connection
	if(listen(sockFD, LIMIT_CONNECT) < 0){
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}
	
	// Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);	
	
	pid_t pid;

	while(1){
		// Takes first connection request and create new socket
		if((new_sockFD = accept(sockFD,(struct sockaddr *)&client_addr,(socklen_t*)&cli_addr_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}
		else {
			// Multi client
			if((pid = fork()) < 0){	// Fork Error
				perror("Fork Error: ");
				exit(EXIT_FAILURE);
			}
			else{
				// Child process
				if((pid = fork()) == 0){
					// Only handle the connection
					close(sockFD);
					
					printf("\n<---\n");
					handle_connect(new_sockFD, &client_addr);
					printf("\n----->\n\n");

					shutdown(new_sockFD,SHUT_RDWR);	// Close the socket
					exit(EXIT_SUCCESS);
				}
				// Parent process
				else{
					close(new_sockFD);
				}
			}
		}
	}
	return 0;
}
