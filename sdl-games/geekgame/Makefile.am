SUBDIRS = graphics

ggzexecmod_PROGRAMS = geekgame

geekgame_SOURCES = intro.c wwwget.c main.c
geekgame_LDADD = $(SDL_LIBS) -lSDL_image -lSDL_ttf -lGL $(LIB_SOUND) $(LIB_GGZ) $(LIB_GGZMOD) $(LIB_XCURSOR)
geekgame_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZMOD_LDFLAGS) $(XCURSOR_LDFLAGS)

noinst_HEADERS = intro.h wwwget.h proto.h

INCLUDES = $(SDL_CFLAGS) $(GGZ_INCLUDES) $(GGZMOD_INCLUDES)

EXTRA_DIST = protocol-description module.dsc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

