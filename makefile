CC ?= clang

all: bin/server bin/client

bin/server: server.c cliparser.h
	$(CC) -o bin/server server.c

bin/client: client.c
	$(CC) -o bin/client client.c

tests:
	$(CC) -o tests.out test.c
