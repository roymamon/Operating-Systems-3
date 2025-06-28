CC = gcc
CFLAGS = -Wall -g

all: ch_merge ch_qsort

ch_merge: convex_hull_2.c
	$(CC) $(CFLAGS) -o ch_merge convex_hull_2.c -DUSE_QSORT=0 -lm

ch_qsort: convex_hull_2.c
	$(CC) $(CFLAGS) -o ch_qsort convex_hull_2.c -DUSE_QSORT=1 -lm

clean:
	rm -f ch_merge ch_qsort