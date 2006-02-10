GGZDIR = `ggz-config -c`/..
GGZFLAGS = -I$(GGZDIR)/include -L$(GGZDIR)/lib -lggz

all:
	gcc -Wall -O2 -o calcrankings main.c \
		-I /usr/include/pgsql -I /usr/include/postgresql \
		-lpq $(GGZFLAGS)

