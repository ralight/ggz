## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2314 2001-08-29 04:00:38Z jdorje $

SUBDIRS = .

DB_SOURCES = ggzdb_@database@.c
DB_OBJECTS = ggzdb_@database@.o

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd entry.room $(DB_SOURCES)

bin_PROGRAMS = ggzd

ggzdroom_DATA = entry.room

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
		ggzdmod.c ggzdmod.h \
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
		net.c \
		net.h \
		room.c \
		room.h \
		transit.c \
		transit.h \
		hash.c \
		hash.h \
		ggzdb.c \
		ggzdb.h \
		login.c \
		login.h \
		lists.c \
		lists.h \
		conf.c \
		conf.h


ggzd_LDADD = $(DB_OBJECTS) $(LDADD) $(ggzd_LIBS)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
