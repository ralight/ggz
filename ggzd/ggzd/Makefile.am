## Process this file with automake to produce Makefile.in

SUBDIRS = .

EXTRA_DIST = ggzd.conf

INCLUDES = -I$(top_srcdir)/easysock 

bin_PROGRAMS = ggzd

ggzd_SOURCES =	control.c \
		datatypes.h \
		err_func.c \
		err_func.h \
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
		transit.h


confdir = $(sysconfdir)
conf_DATA = ggzd.conf

ggzd_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggzd_LDFLAGS = -static


