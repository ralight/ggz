## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6914 2005-02-03 10:04:00Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggz-config

AM_CPPFLAGS = -I$(top_srcdir)/ggzcore/ $(LIBGGZ_INCLUDES)

ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la \
	$(LIB_GGZ)
ggz_config_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggz_config_SOURCES = ggz-config.c
