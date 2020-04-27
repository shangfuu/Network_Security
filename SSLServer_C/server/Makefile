CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto
SSL = openssl

pKey = server-pKey.pem
CERT = server-cert.pem
CSR = server.csr

CA_ROOT = ../CA
CA_cert = /CA-cert.pem
CA_key = /CA-pKey.pem

all: server selfSignedCertificate

server: server.c
	$(CC) -o server.o server.c  $(CFLAGS) $(OPENSSL_FLAGS)

selfSignedCertificate: 
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=Server" -out $(CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(CSR) -CA $(CA_ROOT)$(CA_cert) -CAkey $(CA_ROOT)$(CA_key) -CAcreateserial -out $(CERT)

.PHONY: clean

clean:
	@echo "Cleaning Object Pem CSR file..."
	-rm *.o
	-rm *.pem
	-rm *.csr

