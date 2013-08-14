#
# Author Branislav Blaskovic
#
BIN=dns_stat
CC=g++

ALL: $(BIN).o packet.class.o dns_listener.class.o statistic.class.o
	$(CC) -o $(BIN) $(BIN).o packet.class.o dns_listener.class.o statistic.class.o

clean:
	rm -f $(BIN) 
	rm -f *.o
