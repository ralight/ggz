SUBDIRS = .

ggzexecmod_PROGRAMS = ggz.hastings

ggz_hastings_SOURCES = \
	game.h \
	main.c \
	main_win.c \
	main_win.h \
	support.c \
	support.h \
	ggzintl.c \
	ggzintl.h \
	dlg_about.c \
	dlg_about.h \
	dlg_yesno.c \
	dlg_yesno.h \
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

INCLUDES = @GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc

