all: hash crack

crack:
	gcc -O2 -Wall -Werror -o omnicrack omnicrack.c main.c -licui18n -DWITH_ICU -DDEBUG

hash:
	gcc -O2 -Wall -Werror -o hasher hasher.c

clean:
	rm -f hasher omnicrack

