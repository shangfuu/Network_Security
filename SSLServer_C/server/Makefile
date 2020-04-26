CC = gcc
CFLAGS = -g -w -Wall
OPENSSL_FLAGS = -lssl -lcrypto

all: server

server: server.c
	$(CC) -o server.o server.c  $(CFLAGS) $(OPENSSL_FLAGS)

.PHONY: clean

clean:
	@echo "Cleaning Object file..."
	-rm *.o

