## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3997 2002-04-16 19:03:58Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libclient.a

libclient_a_SOURCES = client.c client.h

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/common $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
