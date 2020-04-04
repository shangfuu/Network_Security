#include "http.h"

#define BUFF_SIZE 4096
#define DEBUG(type, msg) printf(type, msg);

void GET_request();
void POST_request();

int recv_line(char *);

void http_header(int );
void handle_connect(int,struct sockaddr_in *);
void Parsing_CMD(int,char const *[]);


void handle_connect(int new_sockFD, struct sockaddr_in *client_addr){
	
	char request[BUFF_SIZE] = {0};	// Http request
	char resource[BUFF_SIZE] = {0}; // Resource in server
	int length;

	// Read the Client request
	read(new_sockFD, request, BUFF_SIZE);
	
	DEBUG("%s\n",request);
	
	// Read the first line of request
	length = recv_line(request);
	printf("Get request from %s:%d \"%s\"\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), request);

	// Check if HTTP request
	char *token;
	if((token = strstr(request,"HTTP/1.1")) == NULL){
		perror("Not Http/1.1: ");
		exit(EXIT_FAILURE);
	}
	else{
		
		const char* delim = " ";
		// Parsing the request into: Method, URL, Protocol Version
		char *Method = strtok(request, delim);
		char *Url = strtok(NULL,delim);
		char *Protocol = strtok(NULL,delim);

		printf("%s,%s,%s\n",Method,Url,Protocol);

		// Get: method = 1, POST, method = 2
		int method = -1;
		if(strcmp(Method,"GET") == 0)
			method = 1;
		else if(strcmp(Method, "POST") == 0)
			method = 2;


		// Only handle the GET request or POST request
		if(method == -1){
			printf("Unknown Request!!\n");
		}
		else{
			// When url end with '/', set it as "/DEFAULT_PAGE"
			if(Url[strlen(Url)-1] == '/'){
				strcat(Url, DEFAULT_PAGE);
				DEBUG("%s\n",Url);
			}

			// The Url resource
			strcpy(resource,ROOT);
			strcat(resource,Url);	// e.g. "ROOT/xxx/DEFAULT_PAGE"
			
			DEBUG("Resource: %s\n",Url);
			
			// Open the resource file
			int fd = open(resource,O_RDONLY);

			// File not found
			if (fd == -1){
				printf("404 Not Found\n");
				// Send 404 Header and 404.html

			}
			else{
				// Send 200 Header
				// Handle GET request
				// Handle POST request
			}
		}

		// Check if POST Method
	}

	
	// Sent msg to client
	http_header(new_sockFD);
}

void http_header(int new_sockFD){

	char *msg = "<h1>Hello GG</h1>";

	// Version, Status code, Status message
	write(new_sockFD, "HTTP/1.1 200 Ok\r\n",17);
	
	// Content-Type
	write(new_sockFD,"Content-Type: text/html\r\n",25);
	
	// Date
	time_t current = time(NULL);
	char buffer[BUFF_SIZE];
	sprintf(buffer,"Date: %s\r",ctime(&current));
	write(new_sockFD,buffer,strlen(buffer));
	DEBUG("%s", buffer);
	memset(buffer,0,sizeof(buffer));	

	// Server name
	char *server_name = "Server: HttpServerC\r\n";
	write(new_sockFD,server_name,strlen(server_name));
	DEBUG("%s", server_name);
	
	// Content-Length
	sprintf(buffer,"Content-Length: %ld\r\n",strlen(msg));
	write(new_sockFD,buffer,strlen(buffer));
	DEBUG("%s", buffer);
	
	write(new_sockFD,"\r\n",2);
	// Send Message
	write(new_sockFD,msg,strlen(msg));

}

// Parsing the cmd input
void Parsing_CMD(int argc, char const *argv[]){
	if(argc > 2){
		perror("Input format: ./server.o {PORT}\n");
		exit(EXIT_FAILURE);
	}
	else if (argc == 2){
		PORT = atoi(argv[1]);
	}
	DEBUG("Open server at port: %d\n",PORT);
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
		printf("\n+++++ Waiting for new connection +++++\n");
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
					handle_connect(new_sockFD, &client_addr);
					close(new_sockFD);
					exit(EXIT_SUCCESS);	
				}
				// Parent process
				else{
					close(new_sockFD);
				}
			}
		}
		printf("+++++ MSG sent +++++\n");
	}

	return 0;
}
