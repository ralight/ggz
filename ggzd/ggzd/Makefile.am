## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2768 2001-12-01 06:51:06Z bmh $

SUBDIRS = .

DB_ALL = ggzdb_db2.c ggzdb_db3.c 
DB_SOURCES = ggzdb_@database@.c

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd entry.room $(DB_ALL)

bin_PROGRAMS = ggzd ggzduedit

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
		$(DB_SOURCES) \
		login.c \
		login.h \
		lists.c \
		lists.h \
		conf.c \
		conf.h \
		perms.h \
		perms.c 


INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_LDADD = $(LDADD) $(ggzd_LIBS) $(top_builddir)/ggzdmod/libggzdmod.la

ggzduedit_SOURCES = ggzduedit.c \
		    $(DB_SOURCES) \
		    perms.h

ggzduedit_LDADD = $(LDADD) $(ggzd_LIBS)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
