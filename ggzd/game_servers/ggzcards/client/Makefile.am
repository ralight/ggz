## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4649 2002-09-21 20:11:01Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libclient.a

libclient_a_CFLAGS = -DAI_CLIENT
libclient_a_SOURCES = client.c client.h

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/common $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
