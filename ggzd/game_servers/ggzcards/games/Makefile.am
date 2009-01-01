noinst_LIBRARIES = libgames.a

# NOTE: ggzdmod must come last so we don't include "protocol.h" from
# that directory!!  Ugh!
AM_CPPFLAGS = -I$(top_srcdir)/game_servers/ggzcards \
	-I$(top_srcdir)/game_servers/ggzcards/common \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES)

libgames_a_DEPENDENCIES = $(top_builddir)/game_servers/ggzcards/common/libcommon.a

libgames_a_SOURCES =	bridge.c bridge.h	\
			euchre.c euchre.h	\
			fortytwo.c fortytwo.h	\
			hearts.c hearts.h	\
			lapocha.c lapocha.h	\
			spades.c spades.h	\
			suaro.c suaro.h		\
			sueca.c sueca.h		\
			whist.c whist.h
