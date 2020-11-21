CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wextra -pedantic

all: compiler

compiler: lex.c keyword.c compiler.c
	$(CC) $(CFLAGS) -o compiler lex.c keyword.c compiler.c

test: test.c
	$(CC) $(CFLAGS) -o $@ $<

pack:
	zip xcuhan00.zip *.c *.h Makefile	

clean:
	rm -f *.o compiler test 
