## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6257 2004-11-04 22:53:36Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggz-config

AM_CPPFLAGS = -I$(top_srcdir)/ggzcore/

ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la $(top_builddir)/ggzmod/libggzmod.la @LDADD@
ggz_config_SOURCES = ggz-config.c
