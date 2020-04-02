#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8000

void http_request(int *new_socket){
	// msg
	char *msg = "<h1>Hello from server</h1>\n";
	
	// Header part
	char *header = "HTTP/1.1 200 ok\r\n";

	write(*new_socket,header,strlen(header));
	write(*new_socket,"Content-Type: text/html\r\n",25);
	
	char buffer[100];
	sprintf(buffer,"Content-Length: %d\r\n\r\n",strlen(msg));
	write(*new_socket,buffer,strlen(buffer));
	
	write(*new_socket,msg,strlen(msg));
}

void http_request();

int main(int argc,char const *argv[]){
	int server, new_socket;
	struct sockaddr_in address;
	int addr_len = sizeof(address);
	
	// Create new socket
	if((server = socket(AF_INET,SOCK_STREAM,0))==0){
		perror("Socket Error:");
		exit(EXIT_FAILURE);
	}

	// Binding
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	memset(address.sin_zero, '\0',sizeof(address.sin_zero));
	
	if(bind(server,(struct sockaddr *)&address,sizeof(address)) < 0){
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}
	// Ready to accept new connection
	if(listen(server,20) < 0){
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		printf("\n----------Waiting fornew connection----------\n");
		// Takes first connection request and create new socket
		if((new_socket = accept(server,(struct sockaddr *)&address,(socklen_t*)&addr_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		char buffer[10000] = {0};
		// Read msg from client
		read(new_socket,buffer,10000);
		printf("%s\n",buffer);
		// Sent msg to client
		http_request(&new_socket);

		printf("-----------------Hello MSG sent-----------------\n");
		close(new_socket);
	}
	return 0;
}
