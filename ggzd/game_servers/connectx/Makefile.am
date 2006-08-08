ggzdexecmod_PROGRAMS = ggzd.connectx
dist_ggzdgame_DATA = connectx.dsc
dist_ggzdroom_DATA = connectx.room

ggzd_connectx_SOURCES = \
	main.c \
	game.c \
	game.h \
	ai-velena.c \
	ai-velena.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

ggzd_connectx_LDADD = $(LDADD) $(top_builddir)/ggzdmod/libggzdmod.la

