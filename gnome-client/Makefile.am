## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6947 2005-02-27 19:24:08Z josef $

SUBDIRS = src motd-editor pixmaps po

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gnome.desktop motd-editor.desktop

schemadir = $(sysconfdir)/gconf/schemas/
schema_DATA = ggz-gnome.schemas

EXTRA_DIST	=	autogen.sh	\
			indent.pro	\
			m4/ggz.m4	\
			m4/iconv.m4	\
			$(util_DATA)	\
			$(schema_DATA)
