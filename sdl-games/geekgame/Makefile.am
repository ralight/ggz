bin_PROGRAMS = geekgame

geekgame_SOURCES = wwwget.c main.c
geekgame_LDADD = $(SDL_LIBS) -lSDL_image -lSDL_ttf $(LIB_GGZ)

noinst_HEADERS = wwwget.h

INCLUDES = $(SDL_CFLAGS) $(GGZ_INCLUDES)

geekdir = $(ggzdatadir)/geekgame
geek_DATA = bot.png one.png zero.png \
			1979rg__.ttf dark2.ttf

EXTRA_DIST = protocol-description

