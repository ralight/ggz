## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = graphics

ggzexecmod_PROGRAMS = geekgame

geekgame_SOURCES = intro.c wwwget.c main.c
geekgame_LDADD = $(SDL_LIBS) -lSDL_image -lSDL_ttf -lGL $(LIB_SOUND) $(LIB_GGZ) $(LIB_GGZMOD) $(LIB_XCURSOR)
geekgame_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZMOD_LDFLAGS) $(XCURSOR_LDFLAGS)

noinst_HEADERS = intro.h wwwget.h proto.h

AM_CPPFLAGS = $(SDL_CFLAGS) $(GGZ_INCLUDES) $(GGZMOD_INCLUDES)

EXTRA_DIST = protocol-description module.dsc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

