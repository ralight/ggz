## Process this file with automake to produce Makefile.in

GAME_DSC = module.dsc

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ 

ggzexecmod_PROGRAMS = ggz.ttt

ggz_ttt_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h \
		  ggzintl.c \
		  ggzintl.h \
	      x.xpm \
	      o.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_ttt_LDADD = @GTK_LIBS@ $(LDADD)

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --fromfile=$(GAME_DSC) --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=$(GAME_DSC)
