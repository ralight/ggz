## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3425 2002-02-20 03:45:35Z jdorje $

SUBDIRS = .

EXTRA_DIST =


ggzdexecmod_PROGRAMS = ggzd.ggzcards.ai-random \
                       ggzd.ggzcards.ai-spades \
                       ggzd.ggzcards.ai-suaro

INCLUDES = -I../games

EXTRA_SOURCES = aicommon.c aicommon.h \
                game.c game.h \
                main.c \
                client.c client.h \
                protocol.c protocol.h

ggzd_ggzcards_ai_random_SOURCES = $(EXTRA_SOURCES) random.c

ggzd_ggzcards_ai_spades_SOURCES = $(EXTRA_SOURCES) spades.c

ggzd_ggzcards_ai_suaro_SOURCES = $(EXTRA_SOURCES) suaro.c

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
