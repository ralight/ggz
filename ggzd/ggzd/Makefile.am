## Process this file with automake to produce Makefile.in

SUBDIRS = .

DB_SOURCES = ggzdb_@database@.c
DB_OBJECTS = ggzdb_@database@.o

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd $(DB_SOURCES)

bin_PROGRAMS = ggzd

ggzd_SOURCES =	chat.c\
		chat.h\
		control.c \
		daemon.c \
		daemon.h \
		datatypes.h \
		err_func.c \
		err_func.h \
		event.c \
		event.h \
		ggzd.h \
		parse_opt.c \
		parse_opt.h \
		players.c \
		players.h \
		protocols.h \
		seats.c \
		seats.h \
		table.c \
		table.h \
		motd.c \
		motd.h \
		room.c \
		room.h \
		transit.c \
		transit.h \
		hash.c \
		hash.h \
		ggzdb.c \
		ggzdb.h \
		login.c \
		login.h


ggzd_LDADD = $(DB_OBJECTS) $(LDADD) $(ggzd_LIBS)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test

