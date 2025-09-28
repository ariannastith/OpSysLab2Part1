CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=gnu11 -D_DEFAULT_SOURCE

.PHONY: all clean

all: my3proc

my3proc: main3.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f my3proc
