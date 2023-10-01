# Makefile for MPI helloworld code
CC=mpicc
CCFLAGS=-Wall -g -std=c99
# activate for compiler optimizations:
#CCFLAGS=-Wall -O3
LDFLAGS= 
CCFLAGS += -I.

BIN=game
OBJ=game.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $< $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< $(LDFLAGS)

clean:
	rm -f *.o $(BIN)
