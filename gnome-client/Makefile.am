## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6675 2005-01-14 07:01:43Z jdorje $

SUBDIRS = src motd-editor pixmaps po

util_DATA = ggz-gnome.desktop
utildir = $(prefix)/share/gnome/apps/Internet/Internet

schema_DATA = ggz-gnome.schemas
schemadir = $(sysconfdir)/gconf/schemas/

EXTRA_DIST	=	autogen.sh	\
			m4/ggz.m4	\
			m4/iconv.m4	\
			$(util_DATA)	\
			$(schema_DATA)
