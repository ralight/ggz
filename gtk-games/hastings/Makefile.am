## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8431 2006-07-31 23:46:52Z jdorje $

SUBDIRS = . pixmaps

AM_CPPFLAGS = -I$(top_srcdir)/common/ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		@GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = \
	dlg_yesno.c dlg_yesno.h \
	game.h \
	main.c \
	main_win.c main_win.h \
	support.c support.h \
	selector.c selector.h

ggz_hastings_LDADD =	$(top_builddir)/common/libgtk-games.a	\
			@GTK_LIBS@ @INTLLIBS@ $(LDADD)		\
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_hastings_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

noinst_HEADERS = version.in
