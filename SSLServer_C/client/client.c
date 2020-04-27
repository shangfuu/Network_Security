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

#define CA_CERT "../CA/CA-cert.pem"

#define CERT "client-cert.pem"
#define pKEY "client-pKey.pem"

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

	printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
	if ((cert = SSL_get_peer_certificate(ssl)) != NULL) {
		printf("Digital certificate information:\n");
    	line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    	printf("Certificate: %s\n", line);
    	free(line);
    	line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    	printf("Issuer: %s\n", line);
    	free(line);
		X509_free(cert);
	}
  	else {
		printf("No certificate information！\n");
	}

}

void parsing_cmd(int argc, char const *argv[]){
	if (argc > 3){
		perror(" Input format:\n1. ./server.o {hostname} {PORT}\n2. ./sever.o {PORT}\n3. ./server.o");
		exit(EXIT_FAILURE);
	}
	else if (argc == 2){
		PORT = atoi(argv[1]);
	}
	else if (argc == 3){
		PORT = atoi(argv[2]);
		HOST_NAME = argv[1];
	}
	printf("Input format:\n1. ./server.o {hostname} {PORT}\n2. ./sever.o {PORT}\n3. ./server.o\n\n");
	printf("Open server at port: %d\n", PORT);
}

SSL_CTX* create_SSL_CTX(SSL_METHOD *method){
	SSL_CTX * ssl_ctx;
	if((ssl_ctx = SSL_CTX_new(method)) == NULL){
		error("Error SSL CTX create: ");
	}
	return ssl_ctx;
}

void configure_SSL_CTX(SSL_CTX *ctx){

	// Verify client
	// Set to require peer (client) certificate verification
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	// Various bugs workaround should be rather harmless
	SSL_CTX_set_options(ctx, SSL_OP_ALL);

	/* Load the RSA CA certificate into the SSL_CTX structure */
    /* This will allow this client to verify the server's     */
    /* certificate.                                           */
	
	if (SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL) != 1){
		error("Load verify loactions Error");
	}

	// Set the Key and certificate
	if(SSL_CTX_use_certificate_file(ctx, CERT, SSL_FILETYPE_PEM) != 1) {
		error("Use Certificate File Error: ");
	}
	if(SSL_CTX_use_PrivateKey_file(ctx, pKEY, SSL_FILETYPE_PEM) != 1) {
		error("Use Private Key File Error: ");
	}

	// Verify the user private key
	if (!SSL_CTX_check_private_key(ctx)){
    	error("Invalid private key: ");
	}
}

int create_socket(){

	int sockFD;
	struct sockaddr_in server_addr;

	// Create new socket
	if((sockFD = socket(AF_INET,SOCK_STREAM,0))==0){
		perror("Socket Error");
		exit(EXIT_FAILURE);
	}

	// initialize server side address and port info
	memset(&server_addr, '\0',sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

	if (connect(sockFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

	return sockFD;
}

int main(int argc, char const *argv[]){

	parsing_cmd(argc, argv);

	int sockFD, new_sockFD;
	SSL_CTX *ssl_ctx;
	SSL *ssl;
	
	// Initialize OpenSSL
	init_openSSL();

	// Create new SSL_CTX to establish TLS/SSL enabled connections
	SSL_METHOD *method = TLSv1_2_client_method();
	ssl_ctx = create_SSL_CTX(method);

	// Configure the Certificate and Private Key file
	configure_SSL_CTX(ssl_ctx);
	
	// Create socket
	sockFD = create_socket();
	
	// Handle Connection

	ssl = SSL_new(ssl_ctx);
	SSL_set_fd(ssl, sockFD);

	// Create SSL connect
	if(SSL_connect(ssl) != 1){
		ERR_print_errors_fp(stderr);
	}
	else {
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    	ShowCerts(ssl);
	}

	// Read message from server
	char *buf = (char*)malloc(BUFF_SIZE * sizeof(char));
	memset(buf,'\0',BUFF_SIZE);
	SSL_read(ssl, buf, BUFF_SIZE);
	printf("MSG from server:\n%s",buf);
	free(buf);

	// Send message to server
	char msg[BUFF_SIZE];
	printf("Input the message (will sent to server):\n");
	fgets(msg, BUFF_SIZE, stdin);
	SSL_write(ssl, msg, strlen(msg));

	// close connection
	close(sockFD);
	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ssl_ctx);
	clear_openSSL();

	return 0;
}
