## Process this file with automake to produce Makefile.in

EXTRA_DIST = module.dsc.in bitmaps/*.xpm

SUBDIRS = libcgc .

INCLUDES = @GTK_CFLAGS@

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --fromfile=module.dsc --force

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=module.dsc


ggzexecmod_PROGRAMS = ggz.chess

ggz_chess_SOURCES = main_win.c \
                    main_win.h \
                    support.c \
                    support.h \
                    callbacks.c \
                    callbacks.h \
                    chess.h \
                    board.c \
                    board.h \
                    game.c \
                    game.h \
                    net.c \
                    net.h \
                    ggz.c \
                    ggz.h \
                    popup.c \
                    popup.h \
                    main.c

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_chess_LDADD = @GTK_LIBS@ $(LDADD) libcgc/libcgc.a
