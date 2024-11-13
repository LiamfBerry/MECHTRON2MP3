CC = gcc
CFLAGS = -Wall -Wextra -g -lm 
LDFLAGS = -lm

all: main

main: main.o vaderSentiment.o lexicon.o
	$(CC) $(CFLAGS) -o main main.o vaderSentiment.o lexicon.o $(LDFLAGS)

main.o: main.c vaderUtility.h
	$(CC) $(CFLAGS) -c main.c

vaderSentiment.o: vaderSentiment.c lexiconUtility.h
	$(CC) $(CFLAGS) -c vaderSentiment.c

lexicon.o: lexicon.c lexiconUtility.h 
	$(CC) $(CFLAGS) -c lexicon.c

clean:
	rm -f main *.o