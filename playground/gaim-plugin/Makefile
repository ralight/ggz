#!/bin/bash

all: simple.so

simple.so: simple.c
	gcc -Wall -O2 `pkg-config --cflags gtk+-2.0` -c simple.c
	gcc -shared simple.o -o ggz4gaim.so -lggz

clean:
	rm -f *.o *.so

install:
	cp *.so /usr/lib/gaim/

