CC=gcc

all: client server

client: client.c
	$(CC) client.c -lrt -o client

server: server.c
	$(CC) -lrt server.c -o server

clean:
	rm -rf client server