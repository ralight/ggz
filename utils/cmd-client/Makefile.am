## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggz-cmd

ggz_cmd_SOURCES = main.c

ggz_cmd_LDFLAGS = $(GGZCORE_LDFLAGS)
ggz_cmd_LDADD = $(LDADD) $(LIB_GGZCORE)

AM_CPPFLAGS = $(GGZCORE_INCLUDES)

