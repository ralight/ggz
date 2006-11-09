## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8651 2006-11-09 06:47:54Z josef $

SUBDIRS = .

bin_PROGRAMS = motd_editor

motd_editor_SOURCES =	callbacks.c \
			callbacks.h \
			interface.c \
			interface.h \
			main.c

motd_editor_LDADD = $(GGZ_LIBS) $(INTLLIBS) $(LIB_GETTEXT)
motd_editor_CFLAGS = $(GGZ_CFLAGS)

