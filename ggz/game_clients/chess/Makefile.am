## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5286 2002-12-15 10:13:29Z jdorje $

SUBDIRS = libcgc .

INCLUDES = -I$(top_srcdir)/common $(GGZMOD_INCLUDES) @GTK_CFLAGS@

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
		support.c	support.h	\
						\
		bitmaps/bishop_b.xpm		\
		bitmaps/bishop_w.xpm		\
		bitmaps/king_b.xpm		\
		bitmaps/king_w.xpm		\
		bitmaps/knight_b.xpm		\
		bitmaps/knight_w.xpm		\
		bitmaps/pawn_b.xpm		\
		bitmaps/pawn_w.xpm		\
		bitmaps/queen_b.xpm		\
		bitmaps/queen_w.xpm		\
		bitmaps/rook_b.xpm		\
		bitmaps/rook_w.xpm

ggz_chess_LDADD = @GTK_LIBS@ $(LDADD) libcgc/libcgc.a \
			$(top_builddir)/common/libgtk-games.a \
			$(LIB_GGZMOD)
ggz_chess_LDFLAGS = $(GGZMOD_LDFLAGS)

