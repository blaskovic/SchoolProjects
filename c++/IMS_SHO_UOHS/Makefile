CPP=g++

all: main.o
	$(CPP) -o main main.o -lsimlib -lm
	rm -f *.o
test:
	./main
	cat stats.out
clean:
	rm -f main

