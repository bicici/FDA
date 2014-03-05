CC=g++
#CC=icpc
#CFLAGS=-O3 -D_XOPEN_SOURCE -Wall -std=c99 -I. `pkg-config --cflags glib-2.0`
CFLAGS=-O3 -D_XOPEN_SOURCE -Wall -I. `pkg-config --cflags glib-2.0`
LIBS=`pkg-config --libs glib-2.0` -lm -lz

all: fda

fda: fda.o minialloc.o ngram.o sentence.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fda.o: fda.c fda.h procinfo.h foreach.h minialloc.h token.h ngram.h sentence.h
	$(CC) -c $(CFLAGS) $< -o $@

sentence.o: sentence.c sentence.h foreach.h token.h
	$(CC) -c $(CFLAGS) $< -o $@

ngram.o: ngram.c ngram.h token.h foreach.h
	$(CC) -c $(CFLAGS) $< -o $@

heap.o: heap.c heap.h token.h
	$(CC) -c $(CFLAGS) $< -o $@

minialloc.o: minialloc.c minialloc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	-rm -f *.o fda

