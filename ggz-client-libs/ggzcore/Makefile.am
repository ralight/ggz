SUBDIRS = .

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzcore.la

libggzcore_la_CPPFLAGS = -I$(top_srcdir)/ggzmod-ggz \
	$(LIBGGZ_INCLUDES) \
	-DGGZCONFDIR=\"$(ggzconfdir)\" \
	-DPREFIX=\"$(prefix)\"

include_HEADERS = ggzcore.h

libggzcore_la_LIBADD = $(top_builddir)/ggzmod-ggz/libggzmod-ggz.la \
		       $(LIB_GGZ) $(LIBEXPAT)

libggzcore_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 8:1:1

libggzcore_la_SOURCES =	conf.c \
			conf.h \
			init.c \
			game.c \
			game.h \
			gametype.c \
			gametype.h \
			ggzcore.h \
			ggzcore-ggz.h \
			hook.c \
			hook.h \
			module.c \
			module.h \
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
