ggzdexecmod_PROGRAMS = ggzd.ccheckers
dist_ggzdgame_DATA = ccheckers.dsc
dist_ggzdroom_DATA = ccheckers.room

ggzd_ccheckers_SOURCES = main.c \
			 game.c \
			 game.h \
			 ai.c \
			 ai.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES) \
	-DGGZDSTATEDIR=\"$(ggzdstatedir)\"
ggzd_ccheckers_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la -lm

