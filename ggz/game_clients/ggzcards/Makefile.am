## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5207 2002-11-04 07:19:23Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = common client .

INCLUDES =	-I$(top_srcdir)/common/ \
		-I$(top_srcdir)/ggzcards/common \
		-I$(top_srcdir)/ggzcards/client \
		$(GGZMOD_INCLUDES)
CFLAGS = @GTK_CFLAGS@ @CFLAGS@

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ggzcards

ggz_ggzcards_SOURCES = \
	ai.h ai.c \
	animation.c animation.h \
	cb_main.c cb_main.h \
	dlg_bid.c dlg_bid.h \
	dlg_main.c dlg_main.h \
	dlg_options.c dlg_options.h \
	dlg_prefs.c dlg_prefs.h \
	drawcard.c drawcard.h \
	game.c game.h \
	layout.c layout.h \
	main.c main.h \
	preferences.c preferences.h \
	support.c support.h \
	table.c table.h

# NOTE: it is necessary to LDADD the libraries twice to get things to work...
ggz_ggzcards_LDADD = @GTK_LIBS@ $(LDADD) \
	$(top_builddir)/common/libgtk-games.a \
	$(top_builddir)/ggzcards/common/libcommon.a \
	$(top_builddir)/ggzcards/client/libclient.a \
	$(top_builddir)/ggzcards/common/libcommon.a \
	$(top_builddir)/ggzcards/client/libclient.a \
	$(LIB_GGZMOD)
ggz_ggzcards_LDFLAGS = $(GGZMOD_LDFLAGS)

pixmap_DATA =	cards-1.xpm cards-b1.xpm \
		cards-2.xpm cards-b2.xpm \
		cards-3.xpm cards-b3.xpm \
		cards-4.xpm cards-b4.xpm
pixmapdir = $(ggzdatadir)/pixmaps
