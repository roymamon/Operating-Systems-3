CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
TARGET = convex_hull

all: $(TARGET)

$(TARGET): convex_hull.c
	$(CC) $(CFLAGS) -o $(TARGET) convex_hull.c

clean:
	rm -f $(TARGET)