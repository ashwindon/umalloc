CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

all: memgrind.o umalloc.a
	$(CC) memgrind.o umalloc.o -o memgrind

umalloc.o: umalloc.h
	$(CC) $(CFLAGS) umalloc.c

umalloc.a: umalloc.o
	$(AR) libumalloc.a umalloc.o
	$(RANLIB) libumalloc.a

memgrind.o: 
	$(CC) $(CFLAGS) memgrind.c -L../ -lumalloc

clean:
	rm *.o *.a memgrind
