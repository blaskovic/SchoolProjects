CFLAGS=-std=c99 -Wall -lm -pedantic -g -O3
BIN=ifj11
CC=gcc
RM=rm -f

ALL: my_string.o scaner.o parser.o main.o interpreter.o types.o ial.o my_memory.o
	$(CC) $(CFLAGS) -o $(BIN) my_string.o scaner.o parser.o main.o interpreter.o types.o ial.o my_memory.o
	$(RM) *.o

clean:
	$(RM) *.o $(BIN)

