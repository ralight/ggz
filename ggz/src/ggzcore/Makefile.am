
SUBDIRS = .

bin_PROGRAMS = ggz-config

ggz_config_LDADD = libggzcore.la @LDADD@
ggz_config_SOURCES = ggz-config.c

ggzconf_DATA = ggz.modules

EXTRA_DIST = $(ggzconf_DATA)

lib_LTLIBRARIES = libggzcore.la

include_HEADERS = ggzcore.h

libggzcore_la_LIBADD = @LDADD@

libggzcore_la_LDFLAGS = -version-info 1:0:0

libggzcore_la_SOURCES =	conf.c \
			conf.h \
		      	init.c \
			game.c \
			game.h \
		      	gametype.c \
		      	gametype.h \
			hook.c \
			hook.h \
			module.c \
			module.h \
			msg.c \
			msg.h \
			net.h \
			netxml.c \
			player.c \
			player.h \
			protocol.h \
			room.c \
			room.h \
			server.c \
			server.h \
			state.c \
			state.h \
			table.c \
			table.h 
