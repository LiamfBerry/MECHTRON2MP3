CC = gcc
CFLAGS = -Wall -Wextra -g -O3

all: main

main: main.o PSO.o OF.o 
	$(CC) $(CFLAGS) -o main main.o PSO.o OF.o -lm -fopenmp

main.o: main.c utility.h OF_lib.h
	$(CC) $(CFLAGS) -c main.c

PSO.o: PSO.c utility.h
	$(CC) $(CFLAGS) -c PSO.c -lm -fopenmp

OF.o: OF.c OF_lib.h
	$(CC) $(CFLAGS) -c OF.c -lm -fopenmp

clean:
	rm -f main *.o