## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6143 2004-07-17 15:57:24Z josef $

SUBDIRS = .

bin_PROGRAMS = ggz-config

INCLUDES = -I$(top_srcdir)/ggzcore/

ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la $(top_builddir)/ggzmod/libggzmod.la @LDADD@
ggz_config_SOURCES = ggz-config.c
