## Process this file with automake to produce Makefile.in

EXTRA_DIST = hastings.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h \
	      dlg_about.c \
	      dlg_about.h \
	      frame_ll.xpm \
	      frame_lr.xpm \
	      frame_ul.xpm \
	      frame_ur.xpm \
	      mangreen.xpm \
	      manyellow.xpm \
	      manbue.xpm \
	      manred.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_hastings_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@
ggz_hastings_LDFLAGS = -static
