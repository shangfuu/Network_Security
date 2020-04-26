CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto

pkey = server_pKey.pem
CERT = server_cert.pem

all: server

server: server.c
	$(CC) -o server.o server.c  $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: gen clean

gen: 
	$(SSL) genrsa -des3 -out $(pKey) 2048
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=MyOrg/OU=MyUnit/CN=my.domain" -days 365 -out $(CERT)

clean:
	@echo "Cleaning Object file..."
	-rm *.o
	-rm *.pem

