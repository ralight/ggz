## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4027 2002-04-21 01:36:44Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = common client .

INCLUDES = @GTK_CFLAGS@ \
	   -I$(top_srcdir)/common/ \
	   -I$(top_srcdir)/ggzcards/common \
	   -I$(top_srcdir)/ggzcards/client

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ggzcards

ggz_ggzcards_SOURCES = \
	animation.c animation.h \
	cb_main.c cb_main.h \
	dlg_about.c dlg_about.h \
	dlg_bid.c dlg_bid.h \
	dlg_main.c dlg_main.h \
	dlg_options.c dlg_options.h \
	dlg_players.c dlg_players.h \
	dlg_prefs.c dlg_prefs.h \
	drawcard.c drawcard.h \
	game.c game.h \
	layout.c layout.h \
	main.c main.h \
	preferences.c preferences.h \
	support.c support.h \
	table.c table.h \
	cards-b1.xpm cards-b2.xpm cards-b3.xpm cards-b4.xpm \
	cards-1.xpm cards-2.xpm cards-3.xpm cards-4.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@

# NOTE: it is necessary to LDADD the libraries twice to get things to work...
ggz_ggzcards_LDADD = @GTK_LIBS@ $(LDADD) \
	$(top_builddir)/common/libgtk-games.a \
	$(top_builddir)/ggzcards/common/libcommon.a \
	$(top_builddir)/ggzcards/client/libclient.a \
	$(top_builddir)/ggzcards/common/libcommon.a \
	$(top_builddir)/ggzcards/client/libclient.a
