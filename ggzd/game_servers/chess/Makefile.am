EXTRA_DIST = chess.dsc chess.room

SUBDIRS = libcgc

AM_CPPFLAGS = -Ilibcgc -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzdexecmod_PROGRAMS = ggzd.chess
ggzdgame_DATA = chess.dsc
ggzdroom_DATA = chess.room

ggzd_chess_SOURCES = main.c \
	chess.h \
	game.h \
	game.c \
	ai.h \
	ai.c \
	ai-gnuchess.h \
	ai-gnuchess.c

ggzd_chess_LDADD = $(LDADD) libcgc/libcgc.a $(top_builddir)/ggzdmod/libggzdmod.la

