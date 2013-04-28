CC = clang
CFLAGS = -g -Wall -Wextra -std=c99
LIBS = -lz

all: rate_of_change

rate_of_change: rate_of_change.c
	$(CC) $(CFLAGS) rate_of_change.c hashtable.c murmur.c -o rate_of_change $(LIBS)

clean:
	rm -f rate_of_change
