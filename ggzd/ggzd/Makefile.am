## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4150 2002-05-04 23:46:47Z jdorje $

SUBDIRS = .

DBALL = ggzdb_db2.c ggzdb_db3.c ggzdb_pgsql.c ggzdb_mysql.c
DBSOURCES = ggzdb_@database@.c

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd entry.room $(DBALL)

bin_PROGRAMS = ggzd ggzduedit

ggzdroom_DATA = entry.room

ggzd_SOURCES =	chat.c\
		chat.h\
		client.c \
		client.h \
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
		$(DBSOURCES) \
		login.c \
		login.h \
		perms.h \
		perms.c \
		util.c util.h


INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_LDADD = $(LDADD) $(ggzd_LIBS) @LTLIBOBJS@ $(top_builddir)/ggzdmod/libggzdmod.la

ggzduedit_SOURCES = ggzduedit.c \
		    $(DBSOURCES) \
		    perms.h

ggzduedit_LDADD = $(LDADD) $(ggzd_LIBS)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
