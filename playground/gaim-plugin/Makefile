#!/bin/bash

all: simple.so

simple.so:
	gcc -Wall -O2 `pkg-config --cflags gtk+-2.0` -c simple.c
	gcc -shared simple.o -o simple.so

clean:
	rm -f *.o *.so

install:
	cp *.so /usr/lib/gaim/

