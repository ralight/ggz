ggzexecmod_PROGRAMS = geekgame

geekgame_SOURCES = wwwget.c main.c
geekgame_LDADD = $(SDL_LIBS) -lSDL_image -lSDL_ttf -lSDL_mixer $(LIB_GGZ) $(LIB_GGZMOD)

noinst_HEADERS = wwwget.h proto.h

INCLUDES = $(SDL_CFLAGS) $(GGZ_INCLUDES) $(GGZMOD_INCLUDES)

geekdir = $(ggzdatadir)/geekgame
geek_DATA = bot.png one.png zero.png \
			1979rg__.ttf dark2.ttf

EXTRA_DIST = protocol-description module.dsc

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

