#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8000
#define BUFF_SIZE 4096

#define TEST(type,print) printf(type,print);

void http_request(int *new_socket){
	// msg
	char *msg = "<h1>Hello from server</h1>\n";
	
	// Version, Status Code, Status message
	char *header = "HTTP/1.1 200 ok\r\n";	
	write(*new_socket,header,strlen(header));
	// Content-Type
	write(*new_socket,"Content-Type: text/html\r\n",25);

	// Date
	time_t current = time(NULL);
	char buffer[BUFF_SIZE];
	sprintf(buffer,"Date: %s\r",ctime(&current));
	write(*new_socket,buffer,strlen(buffer));
	memset(buffer,0,sizeof(buffer));
	TEST("%s",buffer);

	// Server name
	char *server_name = "Server: HttpServerC\r\n";
	write(*new_socket,server_name,strlen(server_name));
	TEST("%s",server_name);
	
	// Content-Length
	sprintf(buffer,"Content-Length: %ld\r\n",strlen(msg));
	write(*new_socket,buffer,strlen(buffer));
	TEST("%s",buffer);
	
	write(*new_socket,"\r\n",2);
	// Send Message
	write(*new_socket,msg,strlen(msg));
}

void http_request();

int main(int argc,char const *argv[]){
	int sockFD, new_sockFD;
	struct sockaddr_in address;
	int addr_len = sizeof(address);
	
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
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	memset(address.sin_zero, '\0',sizeof(address.sin_zero));
	
	if(bind(sockFD,(struct sockaddr *)&address,sizeof(address)) < 0){
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}
	// Ready to accept new connection
	if(listen(sockFD,20) < 0){
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		printf("\n----------Waiting fornew connection----------\n");
		// Takes first connection request and create new socket
		if((new_sockFD = accept(sockFD,(struct sockaddr *)&address,(socklen_t*)&addr_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		char buffer[10000] = {0};
		// Read msg from client
		read(new_sockFD,buffer,10000);
		printf("%s\n",buffer);
		// Sent msg to client
		http_request(&new_sockFD);

		printf("-----------------Hello MSG sent-----------------\n");
		close(new_sockFD);
	}
	return 0;
}
