## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8741 2006-12-22 05:48:24Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libclient.a

# We want to define AI_CLIENT for this code.  Using libclient_a_CFLAGS seems
# better, but doesn't seem to work for automake<1.5.  So I manipulate CFLAGS
# directly...
# libclient_a_CFLAGS = -DAI_CLIENT
AM_CFLAGS = @CFLAGS@ -DAI_CLIENT

libclient_a_SOURCES = client.c client.h

AM_CPPFLAGS = -I$(srcdir)/../common $(LIBGGZ_INCLUDES)

