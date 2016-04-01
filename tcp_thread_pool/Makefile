CC=gcc
FLAGS=-std=c++11

all: client server

client: client.c
	$(CC) $(FLAGS) client.c -o client

server: server.c
	$(CC) $(FLAGS) -pthread server.c -o server

clean:
	rm -rf client server