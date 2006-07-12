all: libggzchess.a

libggzchess.a: ai.o
	ar cru libggzchess.a ai.o
	ranlib libggzchess.a

ai.o: ai.c
	gcc -Wall -O2 -fPIC -c ai.c

clean:
	rm -f *.a *.o

