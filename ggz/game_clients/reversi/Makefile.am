## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ -I$(top_srcdir)/src/ggzcore

install-data-local:
	@$(NORMAL_INSTALL)
	ggz-config -D --install --fromfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-ggz-config --remove --fromfile=module.dsc

ggzexecmod_PROGRAMS = ggz.reversi

ggz_reversi_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      support.c \
	      support.h \
        options_dlg.c \
        options_dlg.h \
        about_dlg.c \
        about_dlg.h \
	black.xpm \
	dot.xpm \
	white.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_reversi_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@ $(top_builddir)/src/ggzcore/libggzcore.a
