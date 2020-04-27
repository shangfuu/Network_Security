CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto
SSL = openssl

SSC_ROOT = ./self-signed-cert/
pKey = $(SSC_ROOT)client-pKey.pem
CERT = $(SSC_ROOT)client-cert.pem
CSR = $(SSC_ROOT)client.csr

CA_ROOT = ../CA/Real-CA/
CA_cert = $(CA_ROOT)CA-cert.pem
CA_key = $(CA_ROOT)CA-pKey.pem

FAKE_CA_ROOT = ../CA/Fake-CA/
FAKE_CA_cert = $(FAKE_CA_ROOT)Fake-CA-cert.pem
FAKE_CA_key = $(FAKE_CA_ROOT)Fake-CA-pKey.pem

all: client selfSignedCertificate Fake_selfSignedCertificate

client: client.c
	$(CC) -o client.o client.c $(CFLAGS) $(OPENSSL_FLAGS)

selfSignedCertificate: 
	# Create folder to put SSC files
	-rm -rf $(SSC_ROOT)
	mkdir $(SSC_ROOT)
	# Create RSA key pair for server with 2048 bits.
	$(SSL) genrsa -out $(pKey) 2048
	# Generate CSR for server using private key.
	$(SSL) req -new -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=Client" -out $(CSR)
	# Self-sign the CSR using CA certificate.
	$(SSL) x509 -req -days 365 -in $(CSR) -CA $(CA_cert) -CAkey $(CA_key) -CAcreateserial -out $(CERT)

Fake_selfSignedCertificate:
	# Self-sign the CSR using Fake-CA certificate.
	$(SSL) x509 -req -days 365 -in $(CSR) -CA $(FAKE_CA_cert) -CAkey $(FAKE_CA_key) -CAcreateserial -out $(SSC_ROOT)Fake-client-cert.pem
	
.PHONY: clean
clean:
	@echo "Cleaning Object Pem CSR file..."
	-rm *.o
	-rm *.pem
	-rm *.csr
	-rm -rf $(SSC_ROOT)

