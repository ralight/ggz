## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4722 2002-09-26 11:13:59Z dr_maux $

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES)
CFLAGS = @GTK_CFLAGS@ @CFLAGS@

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.reversi

ggz_reversi_SOURCES = \
	about_dlg.c	about_dlg.h \
	game.h \
	main.c \
	main_win.c \
	options_dlg.c	options_dlg.h \
	support.c	support.h \
	black.xpm \
	dot.xpm \
	white.xpm

ggz_reversi_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/common/libgtk-games.a $(LIB_GGZMOD)
ggz_reversi_LDFLAGS = $(GGZMOD_LDFLAGS)

