## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	cards.c cards.h \
	net_common.c net_common.h \
	protocol.c protocol.h \
	shared.h

AM_CPPFLAGS = $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
