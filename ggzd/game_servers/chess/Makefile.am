SUBDIRS = libcgc

ggzdexecmod_PROGRAMS = ggzd.chess
dist_ggzdgame_DATA = chess.dsc
dist_ggzdroom_DATA = chess.room

ggzd_chess_SOURCES = main.c \
	chess.h \
	game.h \
	game.c \
	ai.h \
	ai.c \
	ai-gnuchess.h \
	ai-gnuchess.c

AM_CPPFLAGS = -Ilibcgc -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES) \
	-DGGZDSTATEDIR=\"$(ggzdstatedir)\"
ggzd_chess_LDADD = $(LDADD) libcgc/libcgc.a $(top_builddir)/ggzdmod/libggzdmod.la

