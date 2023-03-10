CC ?= clang

all: server client

server: server.c cliparser.h
	$(CC) -o bin/server server.c

client: client.c
	$(CC) -o bin/client client.c

tests:
	$(CC) -o tests.out test.c
