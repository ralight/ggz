## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3663 2002-03-24 21:10:30Z dr_maux $

SUBDIRS = .

ggzdexecmod_PROGRAMS = ggzd.ggzcards.ai-random \
                       ggzd.ggzcards.ai-spades \
                       ggzd.ggzcards.ai-suaro

INCLUDES = -I$(top_srcdir)/game_servers/ggzcards/games -I$(top_srcdir)/game_servers/ggzcards/client -I$(top_srcdir)/game_servers/ggzcards/common

noinst_LIBRARIES = libggzcardsai.a
libggzcardsai_a_SOURCES = aicommon.c game.c
noinst_HEADERS = aicommon.h game.h

AILIB = $(top_srcdir)/game_servers/ggzcards/client/libclient.a $(top_srcdir)/game_servers/ggzcards/common/libcommon.a libggzcardsai.a $(LDADD)

ggzd_ggzcards_ai_random_SOURCES = main.c random.c
ggzd_ggzcards_ai_random_LDADD = $(AILIB)

ggzd_ggzcards_ai_spades_SOURCES = main.c spades.c
ggzd_ggzcards_ai_spades_LDADD = $(AILIB)

ggzd_ggzcards_ai_suaro_SOURCES = main.c suaro.c
ggzd_ggzcards_ai_suaro_LDADD = $(AILIB)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test

