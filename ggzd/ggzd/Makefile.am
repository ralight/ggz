## Process this file with automake to produce Makefile.in

SUBDIRS = .

EXTRA_DIST = ggzd.conf.in ggzd.test.in ggzd.motd

INCLUDES = -I$(top_srcdir)/easysock 

bin_PROGRAMS = ggzd

ggzd_SOURCES =	chat.c\
		chat.h\
		control.c \
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
		ggzdb_db2.c \
		ggzdb_db2.h


ggzd_LDADD = $(LDADD) $(ggzd_LIBS) $(top_builddir)/easysock/libeasysock.la
ggzd_LDFLAGS = -static

ggzdconf_DATA = ggzd.conf ggzd.motd

test:
	-cp -f ggzd.motd $(top_builddir)/test

