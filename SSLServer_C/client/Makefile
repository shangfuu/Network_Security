CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto

all: client

client: client.c
	$(CC) -o client.o client.c  $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: clean

clean:
	@echo "Cleaning Object file..."
	-rm *.o

