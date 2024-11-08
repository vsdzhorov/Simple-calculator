CFLAGS=-std=gnu11 -O3 -Werror
.PHONY: all clean
all: arith
arith: main.c
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f arith