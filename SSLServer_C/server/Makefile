CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto
SSL = openssl

SSC_ROOT = ./self-signed-cert/
pKey = $(SSC_ROOT)server-pKey.pem
CERT = $(SSC_ROOT)server-cert.pem
CSR = $(SSC_ROOT)server.csr

CA_ROOT = ../CA/Real-CA/
CA_cert = $(CA_ROOT)CA-cert.pem
CA_key = $(CA_ROOT)CA-pKey.pem

all: server selfSignedCertificate

server: server.c
	$(CC) -o server.o server.c $(CFLAGS) $(OPENSSL_FLAGS)

selfSignedCertificate: 
	# Create folder to put SSC files
	-rm -rf $(SSC_ROOT)
	mkdir $(SSC_ROOT)
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=Server" -out $(CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(CSR) -CA $(CA_cert) -CAkey $(CA_key) -CAcreateserial -out $(CERT)

.PHONY: clean

clean:
	@echo "Cleaning Object Pem CSR file..."
	-rm *.o
	-rm *.pem
	-rm *.csr
	-rm -rf $(SSC_ROOT)

