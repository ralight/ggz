SUBDIRS = database meta

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd entry.room

bin_PROGRAMS = ggzd ggzduedit

ggzdroom_DATA = entry.room

ggzd_SOURCES = \
	chat.c \
	chat.h \
	client.c \
	client.h \
	control.c \
	daemon.c \
	daemon.h \
	datatypes.h \
	elo.c \
	elo.h \
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
	stats.c \
	stats.h \
	strsep.c \
	transit.c \
	transit.h \
	hash.c \
	hash.h \
	login.c \
	login.h \
	perms.h \
	perms.c \
	util.c \
	util.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzd/database -I$(top_srcdir)/ggzd/meta -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES) $(HOWL_INCLUDES)

ggzd_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzd_LDADD = $(LDADD) $(ggzd_LIBS) $(LIB_DATABASE) $(top_builddir)/ggzdmod/libggzdmod.la $(LIB_GGZ) $(top_builddir)/ggzd/database/libggzdb.a $(top_builddir)/ggzd/meta/libggzmeta.a $(LIB_HOWL)

ggzduedit_SOURCES = ggzduedit.c perms.h

ggzduedit_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzduedit_LDADD = $(LDADD) $(ggzd_LIBS) $(LIB_DATABASE) $(LIB_GGZ) $(top_builddir)/ggzd/database/libggzdb.a

ggzdconf_DATA = ggzd.conf ggzd.motd

