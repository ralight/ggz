
SUBDIRS = .

bin_PROGRAMS = ggz-txt

ggz_txt_SOURCES = 	game.c \
			game.h \
			input.c \
			input.h \
			loop.c \
			loop.h \
			main.c \
			motd.c \
			motd.h \
			output.c \
			output.h \
			server.c \
			server.h \
			state.c \
			state.h 

ggz_txt_LDADD = $(LIB_GGZCORE) $(LIB_GGZ) $(LIB_READLINE) $(LIB_CURSES)
ggz_txt_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS)

INCLUDES = $(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES)

