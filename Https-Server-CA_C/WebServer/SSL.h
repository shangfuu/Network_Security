#ifndef I
#define I
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>

	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <signal.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
#endif

#ifndef BUFF_SIZE
#define BUFF_SIZE 4096
#endif

/*
* This part of Define should not be change, 
* unless you change the dir or filename.
*/
#define CA_CERT "./CA/CA-cert.pem"
#define SERVER_CERT "./WebServer/self-signed-cert/server-cert.pem"
#define SERVER_pKEY "./WebServer/self-signed-cert/server-pKey.pem"

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

SSL_CTX* create_SSL_CTX(SSL_METHOD *method){
	SSL_CTX * ssl_ctx;
	if((ssl_ctx = SSL_CTX_new(method)) == NULL){
		error("Error SSL CTX create");
	}
	return ssl_ctx;
}

void configure_SSL_CTX(SSL_CTX * ctx){
	// Verify client
	// Set to require peer (client) certificate verification
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, NULL);
	// Various bugs workaround should be rather harmless
	SSL_CTX_set_options(ctx, SSL_OP_ALL);

	// Set the certificate of CA
	if (SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL) != 1){
		error("Load verify loactions Error");
	}

	// Set the Key and certificate of server
	if(SSL_CTX_use_certificate_file(ctx, SERVER_CERT, SSL_FILETYPE_PEM) != 1) {
		error("Use Certificate File Error");
	}
	if(SSL_CTX_use_PrivateKey_file(ctx, SERVER_pKEY, SSL_FILETYPE_PEM) != 1) {
		error("Use Private Key File Error");
	}

	// Verify the private key
	if (!SSL_CTX_check_private_key(ctx)){
		error("Verify private key Error");
	}
}

