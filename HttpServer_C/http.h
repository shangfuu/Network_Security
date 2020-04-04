#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROOT "./WebRoot"
#define DEFAULT_PAGE "index.html"
#define LIMIT_CONNECT 50

#define STATUS_404 "HTTP/1.1 404 Not Found\r\n"
#define STATUS_200 "HTTP/1.1 200 Ok\r\n"

int PORT = 8000;

// Set request as first EOL, and return the strlen
int recv_line(char *request){
	char *EOL = "\r\n";
	int EOL_match = 0;
	char *ptr = request;

	for(int i = 0; i < strlen(request); i++){
		if(request[i] == EOL[EOL_match]){
			EOL_match++;
			if(EOL_match == strlen(EOL)){
				*(ptr+1-strlen(EOL)) = '\0';
				return strlen(request);
			}
		}
		else{
			EOL_match = 0;
		}
		ptr++;
	}
	return 0;	// No EOL found
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
	printf("Open server at port: %d\n",PORT);
}