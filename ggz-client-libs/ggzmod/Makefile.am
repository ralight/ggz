EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzmod.la

libggzmod_la_SOURCES = \
	ggzmod.c \
	ggzmod.h \
	io.c \
	io.h \
	mod.h \
	protocol.h

include_HEADERS = ggzmod.h

libggzmod_la_LIBADD = ../ggzcore/libggzcore.la $(LIB_GGZ)
libggzmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 4:0:0

AM_CPPFLAGS = $(LIBGGZ_INCLUDES) -I$(srcdir)/../ggzcore
