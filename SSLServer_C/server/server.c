#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>


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
	return(EXIT_FAILURE);
}

void configure_CA_pk(SSL_CTX * ctx){

	SSL_CTX_set_ecdh_auto(ctx, 1);

	// Set the Key and certificate
	if(SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) != 1) {
		error("Use Certificate File Error: ");
	}
	if(SSL_CTX_use_PrivateKey_file(ctx,"key.pem",SSL_FILETYPE_PEM) != 1) {
		error("Use Private Key File Error: ");
	}
}

int main(int argc, char const *argv[]){

	int sockFD, new_sockFD;
	SSL_CTX *ssl_ctx;
	SSL *ssl;
	
	// Initialize OpenSSL
	init_openSSL();

	// Create new SSL_CTX to establish TLS/SSL enabled connections
	SSL_METHOD *method = TLSv1_2_server_method();
	if((ssl_ctx = SSL_CTX_new(method)) == NULL){
		perror("Error SSL CTX create: ");
		ERR_print_errors_fp(stderr);
		return(EXIT_FAILURE);
	}

	// Configure the Certificate and Private Key file
	configure_CA_pk(ssl_ctx);
	
	
	

	return 0;
}
