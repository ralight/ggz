## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6871 2005-01-24 03:46:22Z jdorje $

SUBDIRS = src motd-editor pixmaps po

util_DATA = ggz-gnome.desktop
utildir = $(prefix)/share/gnome/apps/Internet/Internet

schema_DATA = ggz-gnome.schemas
schemadir = $(sysconfdir)/gconf/schemas/

EXTRA_DIST	=	autogen.sh	\
			indent.pro	\
			m4/ggz.m4	\
			m4/iconv.m4	\
			$(util_DATA)	\
			$(schema_DATA)
