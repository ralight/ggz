## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3664 2002-03-24 21:13:44Z dr_maux $

SUBDIRS = .

DBALL = ggzdb_db2.c ggzdb_db3.c 
DBSOURCES = ggzdb_@database@.c

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd entry.room $(DBALL)

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
		$(DBSOURCES) \
		login.c \
		login.h \
		perms.h \
		perms.c 


INCLUDES = -I$(top_srcdir)/ggzdmod
ggzd_LDADD = $(LDADD) $(ggzd_LIBS) @LTLIBOBJS@ $(top_builddir)/ggzdmod/libggzdmod.la

ggzduedit_SOURCES = ggzduedit.c \
		    $(DBSOURCES) \
		    perms.h

ggzduedit_LDADD = $(LDADD) $(ggzd_LIBS)

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms
