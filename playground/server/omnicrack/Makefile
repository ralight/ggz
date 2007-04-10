all: hash crack

crack:
	gcc -O2 -Wall -Werror -o omnicrack omnicrack.c hasher.c main.c -licui18n -DWITH_ICU -DDEBUG

hash:
	gcc -O2 -Wall -Werror -o omnicrackdict omnicrackdict.c hasher.c

clean:
	rm -f hasher omnicrack

