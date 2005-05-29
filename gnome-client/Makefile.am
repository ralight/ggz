## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 7260 2005-05-29 11:44:13Z josef $

SUBDIRS = src motd-editor pixmaps po

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gnome.desktop motd-editor.desktop

schemadir = $(sysconfdir)/gconf/schemas/
schema_DATA = ggz-gnome.schemas

EXTRA_DIST	=	autogen.sh	\
			indent.pro	\
			m4/ggz.m4	\
			m4/iconv.m4	\
			$(desktop_DATA)	\
			$(schema_DATA)
