CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto
SSL = openssl

SSC_ROOT = ./self-signed-cert/

C_pKey = $(SSC_ROOT)client-pKey.pem
C_CERT = $(SSC_ROOT)client-cert.pem
C_CSR = $(SSC_ROOT)client.csr

S_pKey = $(SSC_ROOT)server-pKey.pem
S_CERT = $(SSC_ROOT)server-cert.pem
S_CSR = $(SSC_ROOT)server.csr

CA_ROOT = ../CA/Real-CA/
CA_cert = $(CA_ROOT)CA-cert.pem
CA_key = $(CA_ROOT)CA-pKey.pem

FAKE_CA_ROOT = ../CA/Fake-CA/
FAKE_CA_cert = $(FAKE_CA_ROOT)Fake-CA-cert.pem
FAKE_CA_key = $(FAKE_CA_ROOT)Fake-CA-pKey.pem

all: make_dir client server

make_dir:
	# Create folder to put SSC files
	-rm -rf $(SSC_ROOT)
	mkdir $(SSC_ROOT)

server: server.c SSL.h
	$(CC) -o server.o server.c $(CFLAGS) $(OPENSSL_FLAGS)


client: client.c SSL.h
	$(CC) -o client.o client.c $(CFLAGS) $(OPENSSL_FLAGS)


.PHONY: clean Gen_SSC_Server Gen_SSC_Client
clean:
	@echo "Cleaning Object Pem CSR file..."
	-rm *.o
	-rm *.pem
	-rm *.csr
	-rm -rf $(SSC_ROOT)

Gen_SSC_Server:
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(S_pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(S_pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=Server" -out $(S_CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(S_CSR) -CA $(CA_cert) -CAkey $(CA_key) -CAcreateserial -out $(S_CERT)

Gen_SSC_Client:
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(C_pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(C_pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=Client" -out $(C_CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(C_CSR) -CA $(CA_cert) -CAkey $(CA_key) -CAcreateserial -out $(C_CERT)

	# FAKE CA
	# Self-sign the CSR using Fake-CA certificate.
	$(SSL) x509 -req -days 365 -in $(C_CSR) -CA $(FAKE_CA_cert) -CAkey $(FAKE_CA_key) -CAcreateserial -out $(SSC_ROOT)Fake-client-cert.pem
	

