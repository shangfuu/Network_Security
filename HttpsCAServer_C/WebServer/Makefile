CC= gcc
CXX = g++
CFLAGS = -g -W -Wall

OPENSSL_FLAGS = -lssl -lcrypto
SSL = openssl
SSC_ROOT = ./self-signed-cert/

S_pKey = $(SSC_ROOT)server-pKey.pem
S_CERT = $(SSC_ROOT)server-cert.pem
S_CSR = $(SSC_ROOT)server.csr

CA_ROOT = ../CA/
CA_cert = $(CA_ROOT)CA-cert.pem
CA_key = $(CA_ROOT)CA-pKey.pem


all: server CGI_view CGI_insert

server: server.c http.h SSL.h
	$(CC) -o server.o server.c $(CFLAGS) $(OPENSSL_FLAGS)

CGI_view: CGI_view.c
	$(CC) -o view.cgi CGI_view.c $(CFLAGS) $(OPENSSL_FLAGS)

CGI_insert: CGI_insert.c
	$(CC) -o insert.cgi CGI_insert.c $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: clean Gen_SSC_Server
clean:
	@echo "Clean obj and cgi files..."
	-rm *.o
	-rm *.cgi
	-rm *.pem
	-rm *.csr
	-rm -rf $(SSC_ROOT)

Gen_SSC_Server:
	# Create folder to put SSC files
	-mkdir $(SSC_ROOT)
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(S_pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(S_pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=Server/CN=b10615045.com" -out $(S_CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(S_CSR) -CA $(CA_cert) -CAkey $(CA_key) -CAcreateserial -out $(S_CERT)

