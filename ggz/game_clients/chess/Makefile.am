## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5623 2003-07-17 02:27:09Z jdorje $

SUBDIRS = libcgc bitmaps

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		 @GTK_CFLAGS@

EXTRA_DIST = module.dsc.in

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

ggzexecmod_PROGRAMS = ggz.chess

ggz_chess_SOURCES =				\
		board.c		board.h		\
		callbacks.c	callbacks.h	\
		chess.h				\
		game.c		game.h		\
		main.c				\
		main_win.c	main_win.h	\
		net.c		net.h		\
		popup.c		popup.h		\
		support.c	support.h

ggz_chess_LDADD = @GTK_LIBS@ $(LDADD) libcgc/libcgc.a \
			$(top_builddir)/common/libgtk-games.a \
			$(LIB_GGZMOD) $(LIB_GGZ)
ggz_chess_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

