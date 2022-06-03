CC=gcc
CXX=g++
INCLUDEFLAGS=-I libpfc/include

all: tlb-explorer

tlb-explorer: tlb-explorer.c libpfc/libpfc.o
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -o $@ $^

tlb-evset: tlb-evset.c libpfc/libpfc.o
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) -o $@ $^

clean:
	rm -rf tlb-explorer tlb-evset tlb-timing *.png *.txt
