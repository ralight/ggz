## Process this file with automake to produce Makefile.in

EXTRA_DIST = ttt.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ 

ggzexecmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h \
	      x.xpm \
	      o.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ttt_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@



