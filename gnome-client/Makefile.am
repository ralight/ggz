## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6945 2005-02-27 19:06:16Z josef $

SUBDIRS = src motd-editor pixmaps po

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gnome.desktop

schemadir = $(sysconfdir)/gconf/schemas/
schema_DATA = ggz-gnome.schemas

EXTRA_DIST	=	autogen.sh	\
			indent.pro	\
			m4/ggz.m4	\
			m4/iconv.m4	\
			$(util_DATA)	\
			$(schema_DATA)
