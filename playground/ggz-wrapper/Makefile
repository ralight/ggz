CC=gcc
CFLAGS+=-Wall -g -I. $(shell pkg-config glib-2.0 --cflags)
LDFLAGS+=-lggzcore -lreadline -lncurses $(shell pkg-config glib-2.0 --libs)
all: ggz-wrapper
ggz-wrapper: ggz-wrapper.o server.o game.o loop.o server.o
clean:
	rm -f *~
	rm -f *.o
	rm ggz-wrapper
