CC = gcc
CFLAGS = -Wall -pthread -Ipatterns
LIB = libpatterns.a
PATTERN_OBJS = patterns/reactor.o patterns/proactor.o
SERVER_OBJS = server.o

all: $(LIB) server

$(LIB): $(PATTERN_OBJS)
	ar rcs $(LIB) $(PATTERN_OBJS)

patterns/reactor.o: patterns/reactor.c patterns/reactor.h
	$(CC) $(CFLAGS) -c patterns/reactor.c -o patterns/reactor.o

patterns/proactor.o: patterns/proactor.c patterns/proactor.h
	$(CC) $(CFLAGS) -c patterns/proactor.c -o patterns/proactor.o

server: $(SERVER_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJS) $(LIB) -lm

server.o: server.c patterns/reactor.h patterns/proactor.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o patterns/*.o server $(LIB)