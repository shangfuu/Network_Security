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
int PORT = 8000;

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

void parsing_cmd(int argc, char const *argv[]){
	if (argc != 2){
		perror("Input format: ./server.o {PORT}\n");
		exit(EXIT_FAILURE);
	}
	else {
		PORT = atoi(argv[1]);
	}
	printf("Open server at port: %d\n", PORT);
}

SSL_CTX* create_SSL_CTX(SSL_METHOD *method){
	SSL_CTX * ssl_ctx;
	if((ssl_ctx = SSL_CTX_new(method)) == NULL){
		error("Error SSL CTX create: ");
	}
	return ssl_ctx;
}

void configure_SSL_CTX(SSL_CTX * ctx){

	SSL_CTX_set_ecdh_auto(ctx, 1);

	// Set the Key and certificate
	if(SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) != 1) {
		error("Use Certificate File Error: ");
	}
	if(SSL_CTX_use_PrivateKey_file(ctx,"pkey.pem",SSL_FILETYPE_PEM) != 1) {
		error("Use Private Key File Error: ");
	}
	
	// Verify the user private key
	if (!SSL_CTX_check_private_key(ctx)){
    	ERR_print_errors_fp(stderr);
    	exit(EXIT_FAILURE);
	}

}

int create_socket(){

	int sockFD;
	struct sockaddr_in server_addr;

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
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	memset(server_addr.sin_zero, '\0',sizeof(server_addr.sin_zero));
	
	if(bind(sockFD,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		perror("Bind Error: ");
		exit(EXIT_FAILURE);
	}
	// Ready to accept new connection
	if(listen(sockFD, LIMIT_CONNECT) < 0){
		perror("Listen Error: ");
		exit(EXIT_FAILURE);
	}
	
	// Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);
}

int main(int argc, char const *argv[]){

	parsing_cmd(argc, argv);

	int sockFD, new_sockFD;
	SSL_CTX *ssl_ctx;
	SSL *ssl;
	
	// Initialize OpenSSL
	init_openSSL();

	// Create new SSL_CTX to establish TLS/SSL enabled connections
	SSL_METHOD *method = TLSv1_2_server_method();
	ssl_ctx = create_SSL_CTX(method);
	

	// Configure the Certificate and Private Key file
	configure_SSL_CTX(ssl_ctx);
	
	// Create socket
	sockFD = create_socket();
	
	// Handle Connection
	while(1){

		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		SSL *ssl;
		char * buf = (char)malloc(BUFF_SIZE * sizeof(char));

		// Takes first connection request and create new socket
		if((new_sockFD = accept(sockFD,(struct sockaddr *)&client_addr,(socklen_t*)&client_len)) < 0){
			perror("Accept Error: ");
			exit(EXIT_FAILURE);
		}

		// Create a SSL based on ssl_ctx
		ssl = SSL_new(ssl_ctx);
		SSL_set_fd(ssl, new_sockFD);
		
		// Wait for client to do TSL/SSL handshake
		if (SSL_accept(ssl) != 1) {
			ERR_print_errors_fp(stderr);
		}
		else {
			printf("Connect!\n");
			SSL_read(ssl, buf, BUFF_SIZE);
			printf("%s\n",buf);
			SSL_write(ssl, "Hello from server!\n", 19);
		}

		// Close SSL and newsocket
		SSL_shutdown(ssl);
		SSL_free(ssl);
		close(new_sockFD);
	}

	// Close SSL_CTX and sockFD
	close(sockFD);
	SSL_CTX_free(ssl_ctx);
	clear_openSSL();

	return 0;
}
