CC=gcc

all: hello_client_tcp hello_server_tcp

hello_client_tcp: hello_client_tcp.c
	$(CC) hello_client_tcp.c -o hello_client_tcp

hello_server_tcp: hello_server_tcp.c
	$(CC) hello_server_tcp.c -o hello_server_tcp

clean:
	rm -rf hello_client_tcp hello_server_tcp