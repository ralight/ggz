CFLAGS = -fPIC -DDEBUG -DWITH_ICU -O2 -Wall -Werror
LIBS = -licui18n
CC = gcc

prefix = /usr/local

all: libomnicrack.so omnicrackdict omnicrack test

libomnicrack.so: omnicrack.o hasher.o
	#ar cru libomnicrack.a $^
	#ranlib libomnicrack.a
	#gcc -shared -o $@ libomnicrack.a $(LIBS)
	gcc -shared -o $@ $^ $(LIBS)

omnicrack: main.o | libomnicrack.so
	gcc -o $@ $^ -L. -lomnicrack

omnicrackdict: omnicrackdict.o | libomnicrack.so
	gcc -o $@ $^ -L. -lomnicrack

test: test.o | libomnicrack.so
	gcc -o $@ $^ -L. -lomnicrack

install:
	cp omnicrack.h $(prefix)/include
	cp libomnicrack.so $(prefix)/lib
	cp omnicrackdict $(prefix)/bin

clean:
	rm -f omnicrackdict omnicrack test
	rm -f *.o *.a *.so

