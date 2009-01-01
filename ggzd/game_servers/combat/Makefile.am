ggzdexecmod_PROGRAMS = ggzd.combat
dist_ggzdgame_DATA = combat.dsc
dist_ggzdroom_DATA = combat.room

ggzd_combat_SOURCES = main.c \
		      game.c \
		      game.h \
		      combat.h \
		      combat.c

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
ggzd_combat_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la
