## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

bin_PROGRAMS = ggz.dots

ggz_dots_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	game.c game.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_dots_LDADD = @GTK_LIBS@ $(LDADD)

