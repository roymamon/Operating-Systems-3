CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = convex_hull
SRC = convex_hull.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)