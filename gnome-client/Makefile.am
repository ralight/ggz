## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5394 2003-02-04 20:18:52Z dr_maux $

SUBDIRS = src pixmaps po

util_DATA = ggz-gnome.desktop
utildir = $(prefix)/share/gnome/apps/Internet/Internet

schema_DATA = ggz-gnome.schemas
schemadir = $(sysconfdir)/gconf/schemas/

EXTRA_DIST	=	mkinstalldirs	\
			autogen.sh	\
			m4/ggz.m4	\
			$(util_DATA)	\
			$(schema_DATA)
