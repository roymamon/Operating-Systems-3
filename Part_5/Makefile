CC = gcc
CFLAGS = -Wall -g -pthread
INCLUDES = -Ipatterns

LIB = libpatterns.a
PATTERN_OBJS = patterns/reactor.o

all: $(LIB) test_reactor

$(LIB): $(PATTERN_OBJS)
	ar rcs $(LIB) $(PATTERN_OBJS)

patterns/reactor.o: patterns/reactor.c patterns/reactor.h
	$(CC) $(CFLAGS) $(INCLUDES) -c patterns/reactor.c -o patterns/reactor.o

test_reactor: test_reactor.o $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o test_reactor test_reactor.o $(LIB)

test_reactor.o: test_reactor.c patterns/reactor.h
	$(CC) $(CFLAGS) $(INCLUDES) -c test_reactor.c

clean:
	rm -f *.o patterns/*.o $(LIB) test_reactor