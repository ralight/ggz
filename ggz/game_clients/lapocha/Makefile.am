## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@

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
	dlg_about.c dlg_about.h \
	cards-b1.xpm cards-b2.xpm cards-b3.xpm cards-b4.xpm \
	cards.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_lapocha_LDADD = @GTK_LIBS@ $(LDADD)

