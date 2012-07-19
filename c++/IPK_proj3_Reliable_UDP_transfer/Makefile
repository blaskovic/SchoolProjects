#=============================================================================
# Author      : Branislav Blaskovic
#============================================================================
CC=g++

OBJ=rdtclient.o Base64Encoder.o rdtserver.o

.PHONY: build
.PHONY: clean

build: ${OBJ}
	${CC} -o rdtclient rdtclient.o Base64Encoder.o -g
	${CC} -o rdtserver rdtserver.o Base64Encoder.o -g

clean:
	rm -f *.o *.core rdtclient rdtserver
