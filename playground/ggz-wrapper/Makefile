CC=gcc

GGZDIR = /home/ggz/BUILD

CFLAGS+=-Wall -g -I. -I$(GGZDIR)/include $(shell pkg-config glib-2.0 --cflags)
LDFLAGS+=-L$(GGZDIR)/lib -lggzcore -lreadline -lncurses $(shell pkg-config glib-2.0 --libs)

all: ggz-wrapper
ggz-wrapper: ggz-wrapper.o server.o game.o loop.o server.o
clean:
	rm -f *~
	rm -f *.o
	rm -f ggz-wrapper
