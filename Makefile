CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wextra -pedantic

all: compiler

compiler: token.c lex.c keyword.c expr.c precedence.c syntax.c error.c symtable_private.c symtable.c compiler.c
	$(CC) $(CFLAGS) -o compiler token.c lex.c expr.c precedence.c keyword.c syntax.c error.c symtable_private.c symtable.c compiler.c

test: test.c
	$(CC) $(CFLAGS) -o $@ $<

pack:
	zip xcuhan00.zip *.c *.h Makefile	

clean:
	rm -f *.o compiler test 
