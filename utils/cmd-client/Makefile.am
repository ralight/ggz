## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4797 2002-10-07 22:06:59Z dr_maux $

SUBDIRS = .

bin_PROGRAMS = ggz-cmd

ggz_cmd_SOURCES = main.c

ggz_cmd_LDFLAGS = $(GGZCORE_LDFLAGS)
ggz_cmd_LDADD = $(LDADD) $(LIB_GGZCORE)

INCLUDES = $(GGZCORE_INCLUDES)

