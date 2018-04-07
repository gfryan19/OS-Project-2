# This is a Makefile
# Use 'make' or 'make all' or 'make msh' to compile your program.
# Use 'make test' to test your program.

all: msh

msh: always
	gcc -g -Wall -Werror -O msh.c lexer.c stringlist.c -lrt -o msh

test: always
	./msh -x < examples.txt

clean:
	rm -f msh *.o

.PHONY: always
