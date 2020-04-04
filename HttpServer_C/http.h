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

#define ROOT "./"
#define DEFAULT_PAGE "index.html"
#define LIMIT_CONNECT 50

int PORT = 8000;


// Get the first EOL, and return the strlen
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