EXTRA_DIST = reversi.dsc reversi.room

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = 	main.c \
			game.c \
			game.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES) \
	-DGGZDSTATEDIR=\"$(ggzdstatedir)\"
ggzd_reversi_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

