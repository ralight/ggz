## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = game.h \
	      main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h \
		  ggzintl.c \
		  ggzintl.h \
	      dlg_about.c \
	      dlg_about.h \
	      frame_black.xpm \
	      frame_ll.xpm \
	      frame_lr.xpm \
	      frame_ul.xpm \
	      frame_ur.xpm \
	      map.xpm \
	      newmanblue.xpm \
	      newmanyellow.xpm \
	      newmangreen.xpm \
	      newmanred.xpm

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_hastings_LDADD = @GTK_LIBS@ $(LDADD) 

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --fromfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=module.dsc
