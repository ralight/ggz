## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3462 2002-02-25 09:15:28Z jdorje $

SUBDIRS = .

EXTRA_DIST =

ggzdexecmod_PROGRAMS = ggzd.ggzcards.ai-random \
                       ggzd.ggzcards.ai-spades \
                       ggzd.ggzcards.ai-suaro

INCLUDES = -I../games -I../client -I../common

EXTRA_SOURCES = aicommon.c aicommon.h \
                game.c game.h \
                main.c

EXTRA_LDADD = $(LDADD) ../common/libcommon.a ../client/libclient.a

#EXTRA_DEPENDENCIES = ../common/libcommon.a ../client/libclient.a

ggzd_ggzcards_ai_random_SOURCES = $(EXTRA_SOURCES) random.c
#ggzd_ggzcards_ai_random_DEPENDENCIES = $(EXTRA_DEPENDENCIES)
ggzd_ggzcards_ai_random_LDADD = $(EXTRA_LDADD)

ggzd_ggzcards_ai_spades_SOURCES = $(EXTRA_SOURCES) spades.c
#ggzd_ggzcards_ai_spades_DEPENDENCIES = $(EXTRA_DEPENDENCIES)
ggzd_ggzcards_ai_spades_LDADD = $(EXTRA_LDADD)

ggzd_ggzcards_ai_suaro_SOURCES = $(EXTRA_SOURCES) suaro.c
#ggzd_ggzcards_ai_suaro_DEPENDENCIES = $(EXTRA_DEPENDENCIES)
ggzd_ggzcards_ai_suaro_LDADD = $(EXTRA_LDADD)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
