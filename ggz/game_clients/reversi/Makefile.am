## Process this file with automake to produce Makefile.in

EXTRA_DIST = reversi.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ 

ggzexecmod_PROGRAMS = ggz.reversi

ggz_reversi_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      support.c \
	      support.h \
        lists.c \
        lists.h \
        options_dlg.c \
        options_dlg.h \
        about_dlg.c \
        about_dlh.h \
        confio.c \
        confio.h \
	      x.xpm \
	      o.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_reversi_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ 



