SUBDIRS = common client config games ai

EXTRA_DIST = TODO

ggzdexecmod_PROGRAMS = ggzd.ggzcards

ggzd_ggzcards_SOURCES = ai.c ai.h \
			bid.c bid.h \
			common.c common.h \
			deck.c deck.h \
			game.c game.h \
			games.c games.h \
			main.c \
			message.c message.h \
			net.c net.h \
			options.c options.h \
			play.c play.h \
			score.c score.h		\
			team.c team.h \
			types.h

ggzd_ggzcards_DEPENDENCIES = games/libgames.a \
			     common/libcommon.a

# NOTE: ggzdmod must come last so we don't include "protocol.h" from
# that directory!!  Ugh!
AM_CPPFLAGS = -I$(top_srcdir)/game_servers/ggzcards/common \
	   -I$(top_srcdir)/game_servers/ggzcards/games \
	   -I$(top_srcdir)/ggzdmod \
	   $(LIBGGZ_INCLUDES)

ggzd_ggzcards_LDADD = $(LDADD) $(LIB_GCRYPT) games/libgames.a \
	common/libcommon.a \
	$(top_builddir)/ggzdmod/libggzdmod.la

