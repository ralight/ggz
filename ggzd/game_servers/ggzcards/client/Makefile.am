## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3880 2002-04-10 13:53:51Z dr_maux $

SUBDIRS = .

noinst_LIBRARIES = libclient.a

libclient_a_SOURCES = client.h client.c

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/common $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
