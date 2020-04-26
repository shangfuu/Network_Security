CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto

pkey = server_pKey.pem
CERT = server_cert.pem
CONFIG = CSR.cnf
CSR = server_csr.csr

CA_ROOT = ../CA
CA_cert = /CA-cert.pem
CA_key = /CA-pKey.pem

all: server

server: server.c
	$(CC) -o server.o server.c  $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: gen clean

gen: 
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -config $(CONFIG) -key $(pKey) -out $(CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(CSR) -CA $(CA_ROOT)$(CA_cert) -CAKey $(CA_ROOT)$(CA_key) -CAcreateserial -out $(CERT)

clean:
	@echo "Cleaning Object file..."
	-rm *.o
	-rm *.pem

