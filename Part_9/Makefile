CC = gcc
CFLAGS = -Wall -pthread

all: server_proactor

server_proactor: server_proactor.o proactor.o
	$(CC) $(CFLAGS) -o server_proactor server_proactor.o proactor.o -lm

server_proactor.o: server_proactor.c proactor.h
	$(CC) $(CFLAGS) -c server_proactor.c

proactor.o: proactor.c proactor.h
	$(CC) $(CFLAGS) -c proactor.c

clean:
	rm -f *.o server_proactor