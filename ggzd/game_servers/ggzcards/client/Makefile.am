## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4332 2002-08-02 03:35:46Z jdorje $

SUBDIRS = .

CFLAGS = -DAI_CLIENT

noinst_LIBRARIES = libclient.a

libclient_a_SOURCES = client.c client.h

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/common $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
