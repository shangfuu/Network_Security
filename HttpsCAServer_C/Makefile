CC= gcc
CXX = g++
CFLAGS = -g -W -Wall

all: server CGI_view CGI_insert

server: server.c http.h
	$(CC) -o server.o server.c $(CFLAGS)

CGI_view: CGI_view.c
	$(CC) -o view.cgi CGI_view.c $(CFLAGS)

CGI_insert: CGI_insert.c
	$(CC) -o insert.cgi CGI_insert.c $(CFLAGS)

# client: client_TCP.c
#	$(CC) -o client.o client_TCP.c $(CFLAGS)

.PHONY: clean
clean:
	@echo "Clean obj and cgi files..."
	-rm *.o
	-rm *.cgi

