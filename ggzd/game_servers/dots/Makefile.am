ggzdexecmod_PROGRAMS = ggzd.dots
dist_ggzdgame_DATA = dots.dsc
dist_ggzdroom_DATA = dots.room

ggzd_dots_SOURCES = main.c \
		    game.c \
		    game.h \
		    ai.c \
		    ai.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES) \
	-DGGZDSTATEDIR=\"$(ggzdstatedir)\"
ggzd_dots_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

