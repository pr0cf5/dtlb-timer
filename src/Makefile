CC=gcc
CFLAGS=-g
CXX=g++
INCLUDEFLAGS=-I libpfc/include

all: tlb-explorer tlb-evset poc

tlb-explorer: tlb-explorer.c libpfc/libpfc.o
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -o $@ $^

tlb-evset: tlb-evset.c libpfc/libpfc.o
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -o $@ $^

poc: poc.c
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -o $@ $^

clean:
	rm -rf tlb-explorer tlb-evset poc *.png *.txt
