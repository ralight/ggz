SUBDIRS = maps

ggzdexecmod_PROGRAMS = ggzd.hastings
dist_ggzdgame_DATA = hastings.dsc
dist_ggzdroom_DATA = hastings.room

ggzd_hastings_SOURCES =  main.c \
		         game.c \
                         game.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_hastings_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

