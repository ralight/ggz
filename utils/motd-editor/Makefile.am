## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4601 2002-09-17 03:37:40Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/intl $(GNOME_INCLUDEDIR)

bin_PROGRAMS = motd_editor

motd_editor_SOURCES =	callbacks.c \
			callbacks.h \
			interface.c \
			interface.h \
			main.c \
			support.c \
			support.h

motd_editor_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS)

yo:
	echo $(GNOME_LIBDIR)
	echo $(GNOMEUI_LIBS)
	echo $(INTLLIBS)
