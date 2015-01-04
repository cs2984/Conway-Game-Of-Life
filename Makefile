# Problem Set 3
# MPATE-GE 2618
# Makefile for life

CC		= gcc

CFLAGS		= -g -std=c99 -Wall

SOURCES		= life.c inputlib.c

BINARIES	= life

default: 	$(BINARIES)

life:		life.c
		$(CC) $(CFLAGS) -o life $(SOURCES)

clean:		
		rm -f *~ *.o a.out core $(BINARIES)

