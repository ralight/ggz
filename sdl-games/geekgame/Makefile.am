ggzexecmod_PROGRAMS = geekgame

geekgame_SOURCES = wwwget.c main.c
geekgame_LDADD = $(SDL_LIBS) -lSDL_image -lSDL_ttf $(LIB_SOUND) $(LIB_GGZ) $(LIB_GGZMOD) $(LIB_XCURSOR)
geekgame_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZMOD_LDFLAGS) $(XCURSOR_LDFLAGS)

noinst_HEADERS = wwwget.h proto.h

INCLUDES = $(SDL_CFLAGS) $(GGZ_INCLUDES) $(GGZMOD_INCLUDES)

geekdir = $(ggzdatadir)/geekgame
dist_geek_DATA = bot.png one.png zero.png icon.png default.png

EXTRA_DIST = protocol-description module.dsc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

