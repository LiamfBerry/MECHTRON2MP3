CC = gcc
CFLAGS = -Wall -Wextra -g -O3 -fopenmp 

all: main

main: main.o PSO.o OF.o
	$(CC) $(CFLAGS) -o main main.o PSO.o OF.o -lm -lprimesieve

main.o: main.c utility.h OF_lib.h
	$(CC) $(CFLAGS) -c main.c

PSO.o: PSO.c utility.h
	$(CC) $(CFLAGS) -c PSO.c -lm -lprimesieve

OF.o: OF.c OF_lib.h
	$(CC) $(CFLAGS) -c OF.c -lm 

clean:
	rm -f main *.o
	