CC = gcc
CFLAGS = -Wall -g
OBJS = server.o reactor.o

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) -o server $(OBJS)

server.o: server.c reactor.h
	$(CC) $(CFLAGS) -c server.c

reactor.o: reactor.c reactor.h
	$(CC) $(CFLAGS) -c reactor.c

clean:
	rm -f *.o server