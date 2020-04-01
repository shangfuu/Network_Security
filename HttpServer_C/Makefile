CC= gcc
CFLAGS = -g -W -Wall

all: server client
server: server_TCP.c
	$(CC) -o server.o server_TCP.c $(CFLAGS)
client: client_TCP.c
	$(CC) -o client.o client_TCP.c $(CFLAGS)

.PHONY: clean
clean:
	@echo "Clean obj files..."
	-rm *.o

