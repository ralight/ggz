## Process this file with automake to produce Makefile.in

EXTRA_DIST = reversi.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

ggzexecmod_PROGRAMS = ggz.reversi

ggz_reversi_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      support.c \
	      support.h \
	      x.xpm \
	      o.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_reversi_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggz_reversi_LDFLAGS = -static



