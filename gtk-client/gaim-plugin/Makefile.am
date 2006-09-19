## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8583 2006-09-19 15:26:31Z jdorje $

plugindir = $(libdir)/gaim

plugin_LTLIBRARIES = ggzgaim.la

ggzgaim_la_SOURCES = simple.c
ggzgaim_la_LDFLAGS = -module -avoid-version $(GTK_CFLAGS) $(LIBGGZ_LDFLAGS)
ggzgaim_la_LIBADD = $(GTK_LIBS) $(LIB_GGZ)

AM_CPPFLAGS = $(GTK_CFLAGS) $(LIBGGZ_INCLUDES) \
	-DPREFIX=\"$(prefix)\"

EXTRA_DIST = ggz.xpm

