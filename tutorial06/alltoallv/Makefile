# Makefile for MPI helloworld code
CC=mpicc
CCFLAGS=-Wall -g -std=c99
# activate for compiler optimizations:
#CCFLAGS=-Wall -O3
LDFLAGS=
CCFLAGS += -I.

BIN=alltoallv
OBJ=alltoallv.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o $(BIN)
