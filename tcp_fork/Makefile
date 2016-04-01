CC=gcc

all: client server

client: client.c
	$(CC) client.c -o client

server: server.c
	$(CC) -lpthread server.c -o server

clean:
	rm -rf client server