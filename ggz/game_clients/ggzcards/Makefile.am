## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3089 2002-01-12 10:18:48Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ggzcards

ggz_ggzcards_SOURCES = \
	animation.c animation.h \
	main.c main.h \
	support.c support.h \
	dlg_main.c dlg_main.h \
	cb_main.c cb_main.h \
	table.c table.h \
	layout.c layout.h \
	game.c game.h \
	dlg_bid.c dlg_bid.h \
	dlg_about.c dlg_about.h \
	dlg_options.c dlg_options.h \
	dlg_prefs.c dlg_prefs.h \
	cards-b1.xpm cards-b2.xpm cards-b3.xpm cards-b4.xpm \
	cards-1.xpm cards-2.xpm cards-3.xpm cards-4.xpm \
	\
	common.c common.h \
	protocol.c protocol.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ggzcards_LDADD = @GTK_LIBS@ $(LDADD) $(LIB_GGZ)

