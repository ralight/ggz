## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8434 2006-08-01 00:11:14Z jdorje $

plugindir = $(libdir)/gaim

plugin_LTLIBRARIES = ggzgaim.la

ggzgaim_la_SOURCES = simple.c
ggzgaim_la_LDFLAGS = -module -avoid-version $(GTK_CFLAGS) $(LIBGGZ_LDFLAGS)
ggzgaim_la_LIBADD = $(GTK_LIBS) $(LIB_GGZ)

AM_CPPFLAGS = $(GTK_CFLAGS) $(LIBGGZ_INCLUDES)

EXTRA_DIST = ggz.xpm

