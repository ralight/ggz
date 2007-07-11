EXTRA_DIST = tictactoe.dsc tictactoe.room

ggzdexecmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = \
	main.c \
	game.c \
	game.h \
	net.c \
	net.h \
	ttt-ai.c \
	ttt-ai.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

PROTOCOL = tictactoe
SOURCEFILE = net
include $(top_srcdir)/m4/protocol.mk

