prefix = `ggz-config -c`/..

GGZFLAGS = -I$(prefix)/include -L$(prefix)/lib -lggz

all: calcrankings

calcrankings: main.c
	gcc -Wall -O2 -o calcrankings main.c \
		-I /usr/include/pgsql -I /usr/include/postgresql \
		-lpq $(GGZFLAGS)

install: calcrankings
	mkdir -p $(DESTDIR)$(prefix)/bin
	cp calcrankings $(DESTDIR)$(prefix)/bin

