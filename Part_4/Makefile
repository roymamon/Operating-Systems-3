CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = server
SRCS = server.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)