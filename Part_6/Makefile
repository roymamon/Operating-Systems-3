CC = gcc
CFLAGS = -Wall -g -pthread -Ipatterns

all: server

libpatterns.a: patterns/reactor.o
	ar rcs libpatterns.a patterns/reactor.o

server: server.o libpatterns.a
	$(CC) $(CFLAGS) -o server server.o libpatterns.a -lm

server.o: server.c patterns/reactor.h
	$(CC) $(CFLAGS) -c server.c

patterns/reactor.o: patterns/reactor.c patterns/reactor.h
	$(CC) $(CFLAGS) -c patterns/reactor.c -o patterns/reactor.o

clean:
	rm -f *.o server libpatterns.a patterns/*.o