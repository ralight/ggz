## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4768 2002-09-29 13:10:57Z dr_maux $

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
		ggzdb_proto.h \
		$(DBSOURCES) \
		login.c \
		login.h \
		perms.h \
		perms.c \
		util.c util.h


INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzd_LDADD = $(LDADD) $(ggzd_LIBS) $(top_builddir)/ggzdmod/libggzdmod.la $(LIB_GGZ)

ggzduedit_SOURCES = ggzduedit.c \
		    $(DBSOURCES) \
		    perms.h

ggzduedit_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzduedit_LDADD = $(LDADD) $(ggzd_LIBS) $(LIB_GGZ)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
