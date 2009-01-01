ggzdexecmod_PROGRAMS = ggzd.escape
dist_ggzdgame_DATA = escape.dsc
dist_ggzdroom_DATA = escape.room

ggzd_escape_SOURCES = main.c \
		      game.c \
                      game.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_escape_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

