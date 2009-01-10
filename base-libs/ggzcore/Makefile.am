EXTRA_DIST = Doxyfile linker.map

if INSTALL_GGZCORE
lib_LTLIBRARIES = libggzcore.la
include_HEADERS = ggzcore.h ggzcore_mainloop.h
else
noinst_LTLIBRARIES = libggzcore.la
endif

libggzcore_la_CPPFLAGS = -I$(top_srcdir)/libggz \
	-I$(top_srcdir)/ggzmod-ggz \
	-DGGZCONFDIR=\"$(sysconfdir)\" \
	-DPREFIX=\"$(prefix)\"

libggzcore_la_LIBADD = $(top_builddir)/ggzmod-ggz/libggzmod-ggz.la \
		       $(LIB_EXPAT) $(LIB_GETTEXT) \
		       $(top_builddir)/libggz/libggz.la

libggzcore_la_LDFLAGS = -version-info 9:0:0
if SYMBOL_VISIBILITY
libggzcore_la_LDFLAGS += -Wl,--version-script=$(srcdir)/linker.map
libggzcore_la_DEPENDENCIES = $(srcdir)/linker.map
endif

libggzcore_la_SOURCES =	conf.c \
			conf.h \
			init.c \
			game.c \
			game.h \
			gametype.c \
			gametype.h \
			ggzcore.h \
			ggzcore-ggz.h \
			ggzcore_mainloop.h \
			ggzcore_mainloop.c \
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
