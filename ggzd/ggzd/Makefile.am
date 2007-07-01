## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 9162 2007-07-01 10:36:29Z oojah $

SUBDIRS = database meta

EXTRA_DIST = ggzd.conf.example.in ggzd.test.in ggzd.motd entry.room

bin_PROGRAMS = ggzd ggzduedit

ggzdroom_DATA = entry.room
ggzdconf_DATA = ggzd.conf.example ggzd.motd username.policy

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
	reconfiguration.h \
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
	unicode.h \
	unicode.c \
	util.c \
	util.h

ggzd_CPPFLAGS = \
	-I$(top_srcdir)/ggzd/database \
	-I$(top_srcdir)/ggzd/meta \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES) \
	$(AVAHI_INCLUDES) \
	$(ICU_INCLUDES) \
	$(CRACKLIB_INCLUDES) \
	$(OMNICRACKLIB_INCLUDES) \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\" \
	-DGGZDEXECMODDIR=\"$(ggzdexecmoddir)\"

ggzd_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzd_LDADD = \
	$(top_builddir)/ggzd/database/libggzdb.a \
	$(top_builddir)/ggzd/meta/libggzmeta.a \
	$(top_builddir)/ggzdmod/libggzdmod.la \
	$(LIB_EXPAT) \
	$(LIB_GGZ) \
	$(LIB_DATABASE) \
	$(LIB_AVAHI) \
	$(LIB_FAM) \
	$(LIB_PTHREADS) \
	$(LIB_ICU) \
	$(LIB_CRACKLIB) \
	$(LIB_OMNICRACKLIB)

ggzduedit_SOURCES =	datatypes.h	\
			ggzduedit.c 				\
			perms.h perms.c

ggzduedit_CPPFLAGS = \
	-I$(top_srcdir)/ggzd/database \
	-I$(top_srcdir)/ggzdmod \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\" \
	$(LIBGGZ_INCLUDES)

ggzduedit_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggzduedit_LDADD = \
	$(top_builddir)/ggzd/database/libggzdb.a \
	$(ggzd_LIBS) \
	$(LIB_GGZ) \
	$(LIB_DATABASE)

