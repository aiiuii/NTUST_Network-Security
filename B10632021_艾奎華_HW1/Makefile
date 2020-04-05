all: Server env mul

CC = gcc
Server: server.c
	$(CC) -o Server server.c

env: env.cpp
	g++ -o env.cgi env.cpp

mul: mul.cpp
	g++ -o mul.cgi mul.cpp

.PHONY: clean,run
clean:
	rm Server env.cgi

test:
	./Server
