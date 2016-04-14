CC=g++
CXX=g++
CXXFLAGS=-std=c++14

all: client server scan

client: client.c
	$(CC) -g -std=c++14 client.c -o client

server: server.c
	$(CC) -g -std=c++14 server.c -o server

scan: scan.o

clean:
	rm -rf client server
