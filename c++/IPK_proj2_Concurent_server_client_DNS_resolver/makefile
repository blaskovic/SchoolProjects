CLIENT=client
SERVER=server
CC=g++

ALL: $(CLIENT).o $(SERVER).o
	$(CC) -o $(CLIENT) $(CLIENT).o
	$(CC) -o $(SERVER) $(SERVER).o

clean:
	rm -f $(CLIENT) $(SERVER)
	rm -f $(CLIENT).o $(SERVER).o
