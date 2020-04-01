#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
int main(){
	int sock = 0;
	struct sockaddr_in server_addr;

	// Create Socket
	if((sock = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("Socket Error: ");
		exit(EXIT_FAILURE);
	}

	// Binding
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	memset(server_addr.sin_zero,'\0',sizeof(server_addr));

	// Convert IPv4 and IPv6 address to binary from
	if(inet_pton(AF_INET,"127.0.0.1",&(server_addr.sin_addr)) <= 0){
		perror("IP conver Error: ");
		exit(EXIT_FAILURE);
	}

	// Connect to server
	if(connect(sock,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
		perror("Connect Error: ");
		exit(EXIT_FAILURE);
	}
	
	char buffer[1024] = {0};
	char *hello = "Hello from client";
	
	// Sent msg to server
	write(sock,hello,strlen(hello));
	printf("Client msg sent\n");
	// Read msg from server
	read(sock, buffer, 1024);
	printf("%s",buffer);

	close(sock);


	return 0;
}
