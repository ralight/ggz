
CC=gcc
CFLAGS=-g3 -Wall `sdl-config --cflags`
FILES=sdl-video.c esd-audio.c general.c text2gif.c
OUTPUT=sdl-video.o esd-audio.o general.o text2gif.o
LIB=libgame.a
PREFIX=/usr

all:
	rm -f $(LIB)
	$(CC) $(CFLAGS) -c $(FILES)
	ar crv $(LIB) $(OUTPUT)
	ranlib $(LIB)
clean:
	rm -f $(OUTPUT) 
	rm -f $(LIB)
install:
	# There is no installation yet!
uninstall:
	# There is no de-installation yet!
