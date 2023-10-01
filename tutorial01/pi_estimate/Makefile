# Makefile for C helloworld code
CC=gcc
CCFLAGS=-Wall -g
# activate for compiler optimizations:
#CCFLAGS=-Wall -O3
LDFLAGS=
CCFLAGS += -I.

OBJ=pi

all: $(OBJ)

$(OBJ): pi_seq.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o $(OBJ)
