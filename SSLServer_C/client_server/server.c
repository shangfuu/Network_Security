#include "SSL.h"

#define CA_CERT "./CA/Real-CA/CA-cert.pem"

#define CERT "./client_server/self-signed-cert/server-cert.pem"
#define pKEY "./client_server/self-signed-cert/server-pKey.pem"

void parsing_cmd(int argc, char const *argv[]){
	if (argc > 2){
		perror("Input format:\n1. ./server.o {PORT} or\n2. ./server.o");
		exit(EXIT_FAILURE);
	}
	else if(argc == 2) {
		PORT = atoi(argv[1]);
	}
	change_root();
	printf("Input format:\n1. ./server.o {PORT} or\n2. ./server.o\n\n");
	printf("Open server at port: %d\n", PORT);
}

void configure_SSL_CTX(SSL_CTX * ctx){

	// Verify client
	// Set to require peer (client) certificate verification
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	// Various bugs workaround should be rather harmless
	SSL_CTX_set_options(ctx, SSL_OP_ALL);

	// Set the certificate of CA
	if (SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL) != 1){
		error("Load verify loactions Error");
	}

	// Set the Key and certificate of server
	if(SSL_CTX_use_certificate_file(ctx, CERT, SSL_FILETYPE_PEM) != 1) {
		error("Use Certificate File Error");
	}
	if(SSL_CTX_use_PrivateKey_file(ctx, pKEY, SSL_FILETYPE_PEM) != 1) {
		error("Use Private Key File Error");
	}

	// Verify the private key
	if (!SSL_CTX_check_private_key(ctx)){
		error("Verify private key Error");
	}

}

int create_socket(){

	int sockFD;
	struct sockaddr_in server_addr;

	// Create new socket
	if((sockFD = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Socket Error");
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
		perror("Bind Error");
		exit(EXIT_FAILURE);
	}
	// Ready to accept new connection
	if(listen(sockFD, LIMIT_CONNECT) < 0){
		perror("Listen Error");
		exit(EXIT_FAILURE);
	}
	
	// Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);

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
		char *buf = (char*)malloc(BUFF_SIZE * sizeof(char));
		memset(buf,'\0',BUFF_SIZE);

		// Takes first connection request and create new socket
		if((new_sockFD = accept(sockFD,(struct sockaddr *)&client_addr,(socklen_t*)&client_len)) < 0){
			perror("Accept Error");
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
			// Print out the connected client
			char client_ip_str[INET_ADDRSTRLEN];
			inet_ntop( AF_INET, &( client_addr.sin_addr.s_addr ), client_ip_str, INET_ADDRSTRLEN );
			printf ("\nConnection from %s, port %d\n", client_ip_str, client_addr.sin_port);
			ShowCerts(ssl);
			
			// Send msg to client
			char *msg = "Hello from server!\n";
			SSL_write(ssl, msg, strlen(msg));

			// Recieve msg from client
			printf("MSG from client:\n");
			while(SSL_read(ssl, buf, BUFF_SIZE)) {
				printf("%s",buf);
				memset(buf,'\0',BUFF_SIZE);
			}
			free(buf);
			printf("----Client Disconnected----\n");
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
