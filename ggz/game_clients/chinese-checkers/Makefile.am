## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc

SUBDIRS = . pixmaps

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ -I$(top_srcdir)/src/ggzcore

install-data-local:
	@$(NORMAL_INSTALL)
	$(top_srcdir)/src/ggzcore/ggz-config -D --install --fromfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-ggz-config --remove --fromfile=module.dsc

ggzexecmod_PROGRAMS = ggz.ccheckers

ggz_ccheckers_SOURCES = display.c	display.h	\
			game.c		game.h		\
			ggz.c		ggz.h		\
			main.c		main.h		\
			support.c	support.h	\
			protocol.h			\
			main_dlg.c	main_dlg.h	\
			main_cb.c	main_cb.h	\
			about_dlg.c	about_dlg.h	\
			prefs_dlg.c	prefs_dlg.h	\
			prefs_cb.c	prefs_cb.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ccheckers_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ $(top_builddir)/src/ggzcore/libggzcore.a

