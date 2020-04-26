CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto

pKey = client_pKey.pem
CERT = client_pkey.pem

all: client

client: client.c
	$(CC) -o client.o client.c  $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: gen clean

gen: 
	$(SSL) genrsa -des3 -out $(pKey) 2048
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=MyOrg/OU=MyUnit/CN=my.domain" -days 365 -out $(CERT)


clean:
	@echo "Cleaning Object file..."
	-rm *.o
	-rm *.pem

