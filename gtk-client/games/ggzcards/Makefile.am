## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 10350 2008-07-16 01:30:46Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = common client .

AM_CPPFLAGS =	-I$(srcdir)/../common/ \
		-I$(srcdir)/common \
		-I$(srcdir)/client \
		$(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

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
	dlg_scores.c dlg_scores.h \
	drawcard.c drawcard.h \
	game.c game.h \
	layout.c layout.h \
	main.c main.h \
	preferences.c preferences.h \
	support.c support.h \
	table.c table.h

# Note: if library X depends on library Y, then library X must be listed
# first here.  If there's a circular dependency you may have to list the
# libs twice!
ggz_ggzcards_LDADD =	$(builddir)/../common/libgtk-games.a		\
			$(GTK_LIBS) $(INTLLIBS) $(LDADD)			\
			$(builddir)/client/libclient.a	\
			$(builddir)/common/libcommon.a	\
			$(LIB_GGZMOD) $(LIB_GGZ)

ggz_ggzcards_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

noinst_HEADERS = version.m4
