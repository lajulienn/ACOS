CC=g++


all: client server

client: client.c
	$(CC) -g -std=c++14 client.c -o client

server: server.c
	$(CC) -g -std=c++14 server.c -o server

clean:
	rm -rf client server
