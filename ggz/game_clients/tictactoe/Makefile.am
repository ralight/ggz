## Process this file with automake to produce Makefile.in

EXTRA_DIST = ttt.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

ggzmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ttt_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggz_ttt_LDFLAGS = -static



