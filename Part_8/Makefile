CC = gcc
CFLAGS = -Wall -pthread -Ipatterns
LIBDIR = patterns
LIBRARY = libpatterns.a

SERVER_SRC = server_proactor.c
SERVER_OBJ = server_proactor.o
LIB_SRC = $(LIBDIR)/proactor.c $(LIBDIR)/reactor.c
LIB_OBJ = $(LIBDIR)/proactor.o $(LIBDIR)/reactor.o

all: $(LIBRARY) server_proactor

$(LIBRARY): $(LIB_OBJ)
	ar rcs $(LIBRARY) $(LIB_OBJ)

server_proactor: $(SERVER_OBJ) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ) $(LIBRARY) -lm

$(SERVER_OBJ): $(SERVER_SRC) $(LIBDIR)/proactor.h $(LIBDIR)/reactor.h
	$(CC) $(CFLAGS) -c $(SERVER_SRC)

$(LIBDIR)/proactor.o: $(LIBDIR)/proactor.c $(LIBDIR)/proactor.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/proactor.c -o $(LIBDIR)/proactor.o

$(LIBDIR)/reactor.o: $(LIBDIR)/reactor.c $(LIBDIR)/reactor.h
	$(CC) $(CFLAGS) -c $(LIBDIR)/reactor.c -o $(LIBDIR)/reactor.o

clean:
	rm -f *.o $(LIBDIR)/*.o server_proactor $(LIBRARY)