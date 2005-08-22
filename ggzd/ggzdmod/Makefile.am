SUBDIRS = .

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzdmod.la

libggzdmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 4:0:0
libggzdmod_la_LIBADD = $(LIB_GGZ)
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod-ggz.h \
			ggzdmod.c \
			io.c io.h \
			protocol.h mod.h

include_HEADERS = ggzdmod.h
noinst_HEADERS = ggzdmod-ggz.h

AM_CPPFLAGS = $(LIBGGZ_INCLUDES)
