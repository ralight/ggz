
SUBDIRS = .

INCLUDES = @EASYSOCK_INC@

bin_PROGRAMS = ggz-config
ggz_config_LDADD = libggzcore.a $(LDADD)
ggz_config_SOURCES = ggz-config.c

noinst_LIBRARIES = libggzcore.a

libggzcore_a_SOURCES =	conf.c \
			conf.h \
			confio.c \
			confio.h \
		      	init.c \
		      	gametype.c \
		      	gametype.h \
			ggzcore.h \
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
