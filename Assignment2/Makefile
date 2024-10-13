#CC = gcc
#CFLAGS = -g -Wall -Wextra -std=c99

#all: main

#main: main.o mySort.o
#$(CC) $(CFLAGS) -o main main.o mySort.o

#main.o: main.c mySort.h
#$(CC) $(CFLAGS) -c main.c

#mySort.o: mySort.c mySort.h
#$(CC) $(CFLAGS) -c mySort.c

#clean:
#rm -f main *.o
	

CC = gcc
CFLAGS = -O3 -fPIC
TARGET = libmysort.so

SRCS = mySort.c


all: $(TARGET)


$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -shared -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)