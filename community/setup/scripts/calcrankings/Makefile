ggzdir = `ggz-config -c`/..

GGZFLAGS = -I$(ggzdir)/include -L$(ggzdir)/lib -lggz

all: calcrankings

calcrankings: main.c
	gcc -Wall -O2 -o calcrankings main.c \
		-I /usr/include/pgsql -I /usr/include/postgresql \
		-lpq $(GGZFLAGS)

install: calcrankings
	cp calcrankings $(ggzdir)/bin

