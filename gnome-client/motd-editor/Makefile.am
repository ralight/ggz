## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5612 2003-06-26 09:22:52Z dr_maux $

SUBDIRS = .

bin_PROGRAMS = motd_editor

motd_editor_SOURCES =	callbacks.c \
			callbacks.h \
			interface.c \
			interface.h \
			main.c

motd_editor_LDADD = $(GGZ_LIBS) $(INTLLIBS)
motd_editor_CFLAGS = $(GGZ_CFLAGS)

