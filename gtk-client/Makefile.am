## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 9365 2007-11-18 11:00:46Z josef $

if COND_PIDGIN
PIDGIN_DIR = pidgin-plugin
endif

SUBDIRS = ggz-gtk src $(COND_PIDGIN) man po doc
DIST_SUBDIRS = ggz-gtk src pidgin-plugin man po doc

EXTRA_DIST = TODO README.GGZ QuickStart.GGZ ggz-gtk.desktop

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gtk.desktop

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
