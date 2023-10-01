# Makefile for C helloworld code
CC=mpicc
CCFLAGS=-Wall -g -std=c99
# activate for compiler optimizations:
#CCFLAGS=-Wall -O3
LDFLAGS=
CCFLAGS += -I.

OBJ=pi

all: $(OBJ)

$(OBJ): pi_paral.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o $(OBJ)
