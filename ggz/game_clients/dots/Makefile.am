## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ -I$(top_srcdir)/src/ggzcore

install-data-local:
	@$(NORMAL_INSTALL)
	$(top_srcdir)/src/ggzcore/ggz-config -D --install --fromfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-ggz-config --remove --fromfile=module.dsc

ggzexecmod_PROGRAMS = ggz.dots

ggz_dots_SOURCES = \
	main.c main.h \
	support.c support.h \
	dlg_main.c dlg_main.h \
	cb_main.c cb_main.h \
	dlg_opt.c dlg_opt.h \
	cb_opt.c cb_opt.h \
	game.c game.h \
	dlg_about.c dlg_about.h \
	dlg_new.c dlg_new.h \
	cb_new.c cb_new.h \
	dlg_pref.c dlg_pref.h \
	cb_pref.c cb_pref.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_dots_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ $(top_builddir)/src/ggzcore/libggzcore.la

