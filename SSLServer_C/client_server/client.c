#include "SSL.h"

#define CA_CERT "./CA/Real-CA/CA-cert.pem"
#define FAKE_CA_CERT "./CA/Fake-CA/Fake-CA-cert.pem"

#define CERT "./client_server/self-signed-cert/client-cert.pem"
#define pKEY "./client_server/self-signed-cert/client-pKey.pem"

enum {real, fake} CA = real;

void cmd_hint(){
	printf(" Input format:\n1. ./client.o {-r/-f} {hostname} {PORT}\n2. ./client.o {-r/-f} {PORT}\
			\n3. ./client.o {-r/-f}\n4. ./client.o\n");
	printf("Default {hostname}: 127.0.0.1, {PORT}: 8000, {-r/-f}: Real CA\n");
}

void parsing_cmd(int argc, char const *argv[]){
	if (argc > 4){
		cmd_hint();
		exit(EXIT_FAILURE);
	}
	else if (argc == 3){
		PORT = atoi(argv[2]);
	}
	else if (argc == 4){
		PORT = atoi(argv[3]);
		HOST_NAME = argv[2];
	}

	if(argc != 1){
		if (strcmp(argv[1], "-f") == 0){
			CA = fake;
		}
		else if (strcmp(argv[1], "-r") == 0){
			CA = real;
		}
		else{
			cmd_hint();
			exit(EXIT_FAILURE);
		}
	}

	cmd_hint();
	change_root();
	printf("Connect to server at %s port: %d\n\n", HOST_NAME, PORT);
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
	if (CA == real){
		if (SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL) != 1){
			error("Load verify loactions Error");
		}
	} else{
		if (SSL_CTX_load_verify_locations(ctx, FAKE_CA_CERT, NULL) != 1){
			error("Load verify loactions Error");
		}
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
	if((sockFD = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Socket Error");
		exit(EXIT_FAILURE);
	}

	// initialize server side address and port info
	memset(&server_addr, '\0',sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, HOST_NAME, &server_addr.sin_addr) <= 0)
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
	printf("\nInput the message (will sent to server until EOF):\n");
	while(fgets(msg, BUFF_SIZE, stdin) != NULL){
		SSL_write(ssl, msg, strlen(msg));
	}

	// close connection
	close(sockFD);
	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ssl_ctx);
	clear_openSSL();

	return 0;
}
