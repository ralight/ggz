aimoduledir = $(ggzddatadir)/ggzcards
dist_aimodule_DATA = spades.aispec suaro.aispec

# Each of these programs shares a whole set of common code
# in this directory, plus depends on the common and client
# libraries for ggzcards.  They just use a different file
# containing the actual AI code.
ggzdexecmod_PROGRAMS = ggzd.ggzcards.ai-random \
                       ggzd.ggzcards.ai-spades \
                       ggzd.ggzcards.ai-suaro

AM_CPPFLAGS = -I$(top_srcdir)/game_servers/ggzcards/games \
	   -I$(top_srcdir)/game_servers/ggzcards/client \
	   -I$(top_srcdir)/game_servers/ggzcards/common \
	   $(LIBGGZ_INCLUDES)

noinst_LIBRARIES = libggzcardsai.a
libggzcardsai_a_SOURCES = aicommon.c game.c main.c aicommon.h game.h
noinst_HEADERS = aicommon.h game.h

AILIB = $(top_builddir)/game_servers/ggzcards/client/libclient.a \
	$(top_builddir)/game_servers/ggzcards/common/libcommon.a \
	libggzcardsai.a \
	$(LDADD) $(LIB_GGZ)

ggzd_ggzcards_ai_random_SOURCES = random.c
ggzd_ggzcards_ai_random_LDADD = $(AILIB)
ggzd_ggzcards_ai_random_LDFLAGS = $(LIBGGZ_LDFLAGS)

ggzd_ggzcards_ai_spades_SOURCES = spades.c
ggzd_ggzcards_ai_spades_LDADD = $(AILIB)
ggzd_ggzcards_ai_spades_LDFLAGS = $(LIBGGZ_LDFLAGS)

ggzd_ggzcards_ai_suaro_SOURCES = suaro.c
ggzd_ggzcards_ai_suaro_LDADD = $(AILIB)
ggzd_ggzcards_ai_suaro_LDFLAGS = $(LIBGGZ_LDFLAGS)

