## Process this file with automake to produce Makefile.in

EXTRA_DIST = lapocha.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

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
	cb_bid.c cb_bid.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_lapocha_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la
ggz_lapocha_LDFLAGS = -static

