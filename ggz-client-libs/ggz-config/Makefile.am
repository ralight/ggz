## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5943 2004-02-16 17:20:34Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggz-config

INCLUDES = $(top_srcdir)/ggzcore/

ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la @LDADD@
ggz_config_SOURCES = ggz-config.c
