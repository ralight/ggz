## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3997 2002-04-16 19:03:58Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	cards.c cards.h \
	net_common.c net_common.h \
	protocol.c protocol.h \
	shared.h

INCLUDES = $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
