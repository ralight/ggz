
SUBDIRS = .

INCLUDES = @EASYSOCK_INC@

bin_PROGRAMS = ggz-config
ggz_config_LDADD = libggzcore.la $(LDADD)
ggz_config_SOURCES = ggz-config.c
sysconf_DATA = ggz.modules

EXTRA_DIST = $(sysconf_DATA)

lib_LTLIBRARIES = libggzcore.la

include_HEADERS = ggzcore.h

libggzcore_la_LIBADD = @LDADD@ @EASYSOCK_LIB@

libggzcore_la_LDFLAGS = -version-info 0:0:0

libggzcore_la_SOURCES =	conf.c \
			conf.h \
			confio.c \
			confio.h \
		      	init.c \
			game.c \
			game.h \
		      	gametype.c \
		      	gametype.h \
			hook.c \
			hook.h \
			lists.c \
			lists.h \
			memory.c \
			memory.h \
			module.c \
			module.h \
			msg.c \
			msg.h \
			net.c \
			net.h \
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
			table.h \
