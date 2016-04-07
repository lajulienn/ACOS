CC=g++
CFLAGS=-std=c++11
LDLIBS=-lpthread
all: client server

#client: client.cc
#	$(CC) $(FLAGS) client.cc -o client
client: client.o

server: server.o
#server: server.cc
#	$(CC) $(FLAGS) -lpthread server.cc -o server

clean:
	rm -rf client server