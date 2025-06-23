CC=gcc
CFLAGS=-Wall -g

all: test_reactor

test_reactor: reactor.o test_reactor.o
	$(CC) $(CFLAGS) -o test_reactor reactor.o test_reactor.o

reactor.o: reactor.c reactor.h
	$(CC) $(CFLAGS) -c reactor.c

test_reactor.o: test_reactor.c reactor.h
	$(CC) $(CFLAGS) -c test_reactor.c

clean:
	rm -f *.o test_reactor