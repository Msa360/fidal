CC ?= clang

all: bin/server bin/client

bin/server: src/server.c src/cliparser.h
	$(CC) -o bin/server src/server.c

bin/client: src/client.c
	$(CC) -o bin/client src/client.c

tests:
	$(CC) -o tests.out test.c
