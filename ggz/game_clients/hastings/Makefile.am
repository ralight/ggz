## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4722 2002-09-26 11:13:59Z dr_maux $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES)
CFLAGS = @GTK_CFLAGS@ @CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = \
	dlg_about.c	dlg_about.h \
	dlg_yesno.c	dlg_yesno.h \
	game.h \
	main.c \
	main_win.c	main_win.h \
	support.c	support.h \
	frame_black.xpm \
	frame_ll.xpm \
	frame_lr.xpm \
	frame_ul.xpm \
	frame_ur.xpm \
	map.xpm \
	newmanblue.xpm \
	newmanyellow.xpm \
	newmangreen.xpm \
	newmanred.xpm \
	shadow.xpm

ggz_hastings_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/common/libgtk-games.a $(LIB_GGZMOD)
ggz_hastings_LDFLAGS = $(GGZMOD_LDFLAGS)

