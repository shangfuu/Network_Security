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

// #define HOST "YEE.com"

#define STATUS_404 "HTTP/1.1 404 Not Found\r\n"
#define STATUS_200 "HTTP/1.1 200 Ok\r\n"

#define BUFF_SIZE 4096
#define DEBUG(type, msg) printf(type, msg);

int PORT = 8000;

// Structure use to handle the request header
typedef struct {
	char* Method;
	char* Url;
	char* Protocol;
	char* Content_Length;	// Uesd in POST
	char* Content_Type;	// Uesd in POST
	char* Query;	// Uesd in POST
}Header;

char* Find_EOL(char*);
void string_copy(char**,char*);

// Handle the request header
Header *request_header(char request[]){

	Header *header = &(Header) { .Method = "\0", .Url = "\0", .Protocol = "\0",
								 .Content_Length = "\0", .Content_Type="\0", 
								.Query="\0", };

	char* token;
	char *req_cp = (char*)malloc((strlen(request)+1) * sizeof(char));
	strcpy(req_cp, request);

	// Get the Method, Url, Protocol
	token = Find_EOL(req_cp);
	printf("Token: %s\n",token);

	const char* delim = " ";
	// Parsing the request into: Method, URL, Protocol Version
	char *method = strtok(token, delim);
	string_copy(&header->Method,method);

	char *url = strtok(NULL,delim);
	string_copy(&header->Url,url);

	char *protocol = strtok(NULL,delim);
	string_copy(&header->Protocol,protocol);

	if (strcmp(header->Method,"POST") == 0){
		delim = ": ";
		// Content-Length
		if ((token = strstr(req_cp,"Content-Length")) != NULL) {
			token = Find_EOL(token);	// Takes 1 line only
			// Take the sting after ": "
			char* CT = strtok(token, delim);
			CT = strtok(NULL,delim);

			string_copy(&header->Content_Length,CT);
		}
		
		// Content-Type
		if ((token = strstr(req_cp,"Content-Type")) != NULL) {
			token = Find_EOL(token);	// Takes 1 line only
			// Take the sting after ": "
			char* CT = strtok(token, delim);
			CT = strtok(NULL,delim);
			string_copy(&header->Content_Type,CT);
		}

		// CRLF
		if ((token = strstr(req_cp,"\r\n\r\n")) != NULL) {
			// Query
			char* query = strtok(token,"\r\n\r\n");
			string_copy(&header->Query,query);
		}
	}

	free(req_cp);
	return header;
}

void string_copy(char** dest, char* src){
	*dest = (char*)malloc((strlen(src)+1) * sizeof(char));
	strcpy(*dest, src);
}

// Find the sentence before the first EOL
char* Find_EOL(char* sentence) {

	static char buf[BUFF_SIZE] = {0};
	strcpy(buf, sentence);

	char *EOL = "\r\n";
	int EOL_match = 0;

	for(int i = 0; i < strlen(buf); i++){
		if (buf[i] == EOL[EOL_match]) {
			EOL_match++;
			if (EOL_match == strlen(EOL)){
				buf[i+1-strlen(EOL)] = '\0';
				return buf;
			}
		}
		else{
			EOL_match = 0;
		}
	}
	return NULL;
}

// Send full msg and return 0 when fail, 1 when success.
int send_msg(int socket, void *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1) return 0;
        ptr += i;
        length -= i;
    }
    return 1;
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

// Send the corresponding http response header
void response_header(int new_sockFD, char * status){

	// Version, Status code, Status message
	write(new_sockFD, status, strlen(status));
	
	// Content-Type
	write(new_sockFD,"Content-Type: text/html\r\n",25);
	
	// Date
	time_t current = time(NULL);
	char buffer[BUFF_SIZE];
	sprintf(buffer,"Date: %s\r",ctime(&current));
	write(new_sockFD,buffer,strlen(buffer));
	memset(buffer,0,sizeof(buffer));	

	// Server name
	char *server_name = "Server: HttpServerC\r\n";
	write(new_sockFD,server_name,strlen(server_name));

	// Content-Length
//	sprintf(buffer,"Content-Length: %ld\r\n",strlen(msg));
//	write(new_sockFD,buffer,strlen(buffer));
//	DEBUG("%s", buffer);
	
	write(new_sockFD,"\r\n",2);
}


