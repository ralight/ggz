## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8817 2007-01-04 23:33:33Z jdorje $

if COND_GAIM
GAIM_DIR = gaim-plugin
endif

SUBDIRS = ggz-gtk src $(COND_GAIM) man po doc
DIST_SUBDIRS = ggz-gtk src gaim-plugin man po doc

EXTRA_DIST = TODO README.GGZ QuickStart.GGZ ggz-gtk.desktop

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gtk.desktop

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
