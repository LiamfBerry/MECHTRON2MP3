CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99

all: main

main: main.o mySort.o
	$(CC) $(CFLAGS) -o main main.o mySort.o

main.o: main.c mySort.h
	$(CC) $(CFLAGS) -c main.c

mySort.o: mySort.c mySort.h
	$(CC) $(CFLAGS) -c mySort.c

clean:
	rm -f main *.o
	