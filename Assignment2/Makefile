CC = gcc
CFLAGS = -O3 -fPIC

all: libmysort.so

libmysort.so: mySort.c
	$(CC) $(CFLAGS) -shared -o libmysort.so mySort.c

clean:
	rm -f libmysort.so

