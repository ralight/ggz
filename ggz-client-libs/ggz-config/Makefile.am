## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6556 2004-12-22 16:38:28Z josef $

SUBDIRS = .

bin_PROGRAMS = ggz-config

AM_CPPFLAGS = -I$(top_srcdir)/ggzcore/ $(LIBGGZ_INCLUDES)

ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la \
	$(top_builddir)/ggzmod/libggzmod.la \
	$(LIB_GGZ)
ggz_config_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggz_config_SOURCES = ggz-config.c
