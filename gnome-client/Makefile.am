## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5296 2002-12-17 08:53:47Z jdorje $

SUBDIRS = src pixmaps

util_DATA = ggz-gnome.desktop
utildir = $(prefix)/share/gnome/apps/Internet/Internet

schema_DATA = ggz-gnome.schemas
schemadir = $(sysconfdir)/gconf/schemas/

EXTRA_DIST	=	mkinstalldirs	\
			autogen.sh	\
			m4/ggz.m4	\
			$(util_DATA)	\
			$(schema_DATA)
