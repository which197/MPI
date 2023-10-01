# Makefile for MPI helloworld code
CC=mpicc
CCFLAGS=-Wall -g
# activate for compiler optimizations:
#CCFLAGS=-Wall -O3
LDFLAGS=
CCFLAGS += -I.

all: hello

hello: hello.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o hello
