EXTRA_DIST = Doxyfile linker.map

if INSTALL_GGZDMOD
lib_LTLIBRARIES = libggzdmod.la
include_HEADERS = ggzdmod.h
else
noinst_LTLIBRARIES = libggzdmod.la
endif

libggzdmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 7:0:0
if SYMBOL_VISIBILITY
libggzdmod_la_LDFLAGS += -Wl,--version-script=$(srcdir)/linker.map
libggzdmod_la_DEPENDENCIES = $(srcdir)/linker.map
endif
libggzdmod_la_LIBADD = $(LIB_GGZ)
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod-ggz.h \
			ggzdmod.c \
			io.c io.h \
			protocol.h mod.h

AM_CPPFLAGS = $(LIBGGZ_INCLUDES)
