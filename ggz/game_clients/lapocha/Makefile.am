## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3785 2002-04-06 18:45:58Z jdorje $

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/common/

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.lapocha

ggz_lapocha_SOURCES = \
	main.c main.h \
	support.c support.h \
	dlg_main.c dlg_main.h \
	cb_main.c cb_main.h \
	table.c table.h \
	hand.c hand.h \
	game.c game.h \
	dlg_bid.c dlg_bid.h \
	cb_bid.c cb_bid.h \
	dlg_about.c dlg_about.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_lapocha_LDADD = @GTK_LIBS@ $(top_builddir)/common/libgtk-games.a $(LDADD)

