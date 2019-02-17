CC = gcc -Wall -pedantic -std=c99
.PHONY: clean

fitz: game.o grid.o parser.o player.o tiles.o token.o fitz.o
		$(CC) game.o grid.o parser.o player.o tiles.o token.o fitz.o -g -o fitz
game.o:
		$(CC) -c game.c
grid.o:
		$(CC) -c grid.c
parser.o:
		$(CC) -c parser.c
player.o:
		$(CC) -c player.c
tiles.o:
		$(CC) -c tiles.c
token.o:
		$(CC) -c token.c
fitz.o:
		$(CC) -c fitz.c
clean:
	rm -f fitz *.o
