png2term: png2term.c
	gcc -ansi -pedantic -O3 -Wall -Wextra -Werror \
	png2term.c -lpng -opng2term && strip png2term
