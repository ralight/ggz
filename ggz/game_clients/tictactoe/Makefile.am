## Process this file with automake to produce Makefile.in

GAME_DSC = ttt.dsc

EXTRA_DIST = $(GAME_DSC)

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ @EASYSOCK_INC@ 

ggzexecmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h \
	      x.xpm \
	      o.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ttt_LDADD = @GTK_LIBS@ $(LDADD) @EASYSOCK_LIB@

install-data-local:
	@$(NORMAL_INSTALL)
	$(top_srcdir)/src/ggzcore/ggz-config -D --install --fromfile=$(GAME_DSC) --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-ggz-config --remove --fromfile=$(GAME_DSC)
