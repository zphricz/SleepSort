CFLAGS=-O3 -Werror
LDLIBS=-lpthread
CC=gcc

sleepsort: sleepsort.c
	gcc $(CFLAGS) sleepsort.c $(LDLIBS) -o sleepsort

clean:
	rm -f sleepsort
