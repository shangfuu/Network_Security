#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8000
int main(int argc,char const *argv[]){
	int server, new_socket;
	struct sockaddr_in address;
	int addr_len = sizeof(address);

	char *hello = "Hello from server";
	

	if((server = socket(AF_INET,SOCK_STREAM,0))==0){
		perror("Socket Error:");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	memset(address.sin_zero, '\0',sizeof(address.sin_zero));
	
	if(bind(server,(struct sockaddr *)&address,sizeof(address)) < 0){
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}
	if(listen(server,20) < 0){
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		printf("\n----------Waiting fornew connection----------\n");
		if((new_socket = accept(server,(struct sockaddr *)&address,(socklen_t*)&addr_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		char buffer[10000] = {0};
		long context = read(new_socket,buffer,10000);
		printf("%s\n",buffer);
		write(new_socket,hello, strlen(hello));
		printf("-----------------Hello MSG sent-----------------");
		close(new_socket);
	}
	return 0;
}
