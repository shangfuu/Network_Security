#include "http.h"
#include "SSL.h"

void CGI(SSL*,char*,char* , Header);
void handle_connect(SSL*, struct sockaddr_in *);

void CGI(SSL* ssl, char* cgi_type, char* file_name, Header header) {
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
			write(cgiSTDIN[1], file_name, strlen(file_name));
			
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
				SSL_write(ssl, &c, 1);
			}
			// output the message to terminal, and send to socket
			send(STDOUT_FILENO, "\n", 1, 0);
			SSL_write(ssl,"\n",1);

            // connection finish
            close(cgiSTDOUT[0]);
            close(cgiSTDIN[1]);
            waitpid(pid, &status, 0);
		}
	}
}

void handle_connect(SSL* ssl, struct sockaddr_in *client_addr){
	
	char request[BUFF_SIZE] = {0};	// Http request
	char resource[BUFF_SIZE] = {0}; // Resource in server

	// Recieve the Client request
	SSL_read(ssl, request, BUFF_SIZE);
	
	printf("------Request Header:------\n%s \n",request);
	
	// Parsing the request header
	Header header = request_header(request);

	printf("--DEBUG--\nGet request from %s:%d \"%s %s %s\"\n------\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), header.Method,header.Url,header.Protocol);

	// Check if HTTP request
	if((strcmp(header.Protocol,"HTTP/1.1")) != 0){
		perror("Not Http/1.1: ");
		exit(EXIT_FAILURE);
	}
	else{
		/* Default:
		*   Get: method = 1, POST: method = 2. 
		*  File Copy:
		*   Get: method = 3
		*/
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
			
			printf("Header: %s, %s\n", header.Url, FILE_PAGE);

			// File Copy
			if (strcmp(header.Url, FILE_PAGE) == 0){
				if (method == 1)
					method = 3;
			}

			// The Url resource
			strcpy(resource,WEBROOT);
			strcat(resource,header.Url);	// e.g. "ROOT/xxx/DEFAULT_PAGE"
			
			printf("Request page: %s\n", resource);

			// Open the resource file
			int fd = open(resource, O_RDONLY);

			// File not found, cause 404
			if (fd == -1){
				printf("404 Not Found\n");
				// Send 404 Header
				response_header(ssl, STATUS_404);

				// Open the 404.html
				memset(resource, 0, BUFF_SIZE);
				strcpy(resource,WEBROOT);
				strcat(resource,"/404.html");

				// Send the 404.html
				CGI(ssl, VIEW_CGI, resource, header);
			}
			else{
				// Send 200 Header
				response_header(ssl,STATUS_200);

				// Handle Default GET request
				if(method == 1){
					CGI(ssl, VIEW_CGI, resource, header);
				}
				// Handle Default POST request
				else if(method == 2){
					CGI(ssl, INSERT_CGI, resource, header);
				}
				// Handle FileCopy GET request
				else if (method == 3){
					CGI(ssl, FILE_CGI, resource, header);
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
	SSL_CTX *ssl_ctx;
	
	// Initialize OpenSSL
	init_openSSL();

	// Create new SSL_CTX to establish TLS/SSL enabled connections
	SSL_METHOD *method = TLS_server_method();
	ssl_ctx = create_SSL_CTX(method);
	
	// Configure the Certificate and Private Key file
	configure_SSL_CTX(ssl_ctx);

	// Create socket
	sockFD = create_socket();
	
	pid_t pid;
	while(1){

		struct sockaddr_in client_addr;
		int cli_addr_len = sizeof(client_addr);
		

		// Takes first connection request and create new socket
		if((new_sockFD = accept(sockFD,(struct sockaddr *)&client_addr,(socklen_t*)&cli_addr_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		// Multi client
		if((pid = fork()) < 0){	// Fork Error
			perror("Fork Error: ");
			exit(EXIT_FAILURE);
		}
		else{
			// Child process
			if((pid = fork()) == 0){

				SSL *ssl;

				// SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

				// Create a SSL based on ssl_ctx and set fd
				ssl = SSL_new(ssl_ctx);
				SSL_set_fd(ssl, new_sockFD);

				// Do TSL/SSL handshake
				if (SSL_accept(ssl) != 1) {
					ERR_print_errors_fp(stderr);
					ERR_clear_error();
				}
				else {
					handle_connect(ssl, &client_addr);
				}

				close(sockFD);
				SSL_shutdown(ssl);
				SSL_free(ssl);
				shutdown(new_sockFD, SHUT_RDWR);	// Close the socket
				exit(EXIT_SUCCESS);
			}
			// Parent process
			else{
				// Close newsocket
				close(new_sockFD);
			}
		}
	}
	// Close SSL_CTX and sockFD
	close(sockFD);
	SSL_CTX_free(ssl_ctx);
	clear_openSSL();

	return 0;
}
