## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5217 2002-11-04 15:25:52Z dr_maux $

SUBDIRS = .

noinst_LIBRARIES = libclient.a

# We want to define AI_CLIENT for this code.  Using libclient_a_CFLAGS seems
# better, but doesn't seem to work for automake<1.5.  So I manipulate CFLAGS
# directly...
# libclient_a_CFLAGS = -DAI_CLIENT
AM_CFLAGS = @CFLAGS@ -DAI_CLIENT

libclient_a_SOURCES = client.c client.h

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/common $(LIBGGZ_INCLUDES)

test: # nothing to be done (ggz-server only)
