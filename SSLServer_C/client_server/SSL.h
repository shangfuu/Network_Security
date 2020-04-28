#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define LIMIT_CONNECT 20
#define BUFF_SIZE 4096
#define ROOT "SSLServer_C"

int PORT = 8000;
char* HOST_NAME = "127.0.0.1";

void init_openSSL(){
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
}

void clear_openSSL(){
	ERR_free_strings();
	EVP_cleanup();
}

void error(char *msg){
	perror(msg);
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
}

void ShowCerts(SSL * ssl)
{
	X509 *cert;
	char *line;
	
	if ((cert = SSL_get_peer_certificate(ssl)) != NULL) {
		printf("------Show Certificates-------\n");
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));

		printf("Digital certificate information:\n");
    	line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    	printf("Certificate: %s\n", line);
    	free(line);
    	line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    	printf("Issuer: %s\n", line);
    	free(line);
		X509_free(cert);
		printf("-----------------------------\n\n");
	}
  	else {
		printf("----No certificate information！-----\n");
	}
}

void change_root(){
	char * tok;
	char *delim = "/";
	char buf[BUFF_SIZE], dir[BUFF_SIZE];

	// Get the current dir name
	tok = strtok(getcwd(buf,BUFF_SIZE), delim);
	while(tok != NULL){
		strcpy(dir, tok);
		tok = strtok(NULL, delim);
	}

	// Back to working root dir if not in ROOT
	if (strcmp(ROOT, dir) != 0) {
		chdir("..");
	}

}

SSL_CTX* create_SSL_CTX(SSL_METHOD *method){
	SSL_CTX * ssl_ctx;
	if((ssl_ctx = SSL_CTX_new(method)) == NULL){
		error("Error SSL CTX create");
	}
	return ssl_ctx;
}


