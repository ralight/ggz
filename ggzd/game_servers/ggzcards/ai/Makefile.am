## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

aimoduledir = $(ggzddatadir)/ggzcards
aimodule_DATA = spades.aispec suaro.aispec

EXTRA_DIST = $(aimodule_DATA)

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
libggzcardsai_a_CFLAGS = -DAI_CLIENT
noinst_HEADERS = aicommon.h game.h

AILIB = $(top_builddir)/game_servers/ggzcards/client/libclient.a \
	$(top_builddir)/game_servers/ggzcards/common/libcommon.a \
	libggzcardsai.a \
	$(LDADD) $(LIB_GGZ)

#LDFLAGS = $(LIBGGZ_LDFLAGS)

ggzd_ggzcards_ai_random_SOURCES = random.c
ggzd_ggzcards_ai_random_LDADD = $(AILIB)
ggzd_ggzcards_ai_random_CFLAGS = -DAI_CLIENT
ggzd_ggzcards_ai_random_LDFLAGS = $(LIBGGZ_LDFLAGS)

ggzd_ggzcards_ai_spades_SOURCES = spades.c
ggzd_ggzcards_ai_spades_LDADD = $(AILIB)
ggzd_ggzcards_ai_spades_CFLAGS = -DAI_CLIENT
ggzd_ggzcards_ai_spades_LDFLAGS = $(LIBGGZ_LDFLAGS)

ggzd_ggzcards_ai_suaro_SOURCES = suaro.c
ggzd_ggzcards_ai_suaro_LDADD = $(AILIB)
ggzd_ggzcards_ai_suaro_CFLAGS = -DAI_CLIENT
ggzd_ggzcards_ai_suaro_LDFLAGS = $(LIBGGZ_LDFLAGS)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test

