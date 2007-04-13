prefix = /usr/local

all: hash crack lib

crack:
	gcc -O2 -Wall -Werror -o omnicrack omnicrack.c hasher.c main.c -licui18n -DWITH_ICU -DDEBUG

lib:
	gcc -shared -o libomnicrack.so omnicrack.c hasher.c -licui18n -DWITH_ICU -DDEBUG

install:
	cp omnicrack.h $(prefix)/include
	cp libomnicrack.so $(prefix)/lib

hash:
	gcc -O2 -Wall -Werror -o omnicrackdict omnicrackdict.c hasher.c

clean:
	rm -f hasher omnicrack

