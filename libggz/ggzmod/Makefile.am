EXTRA_DIST = Doxyfile

if INSTALL_GGZMOD
lib_LTLIBRARIES = libggzmod.la
include_HEADERS = ggzmod.h
else
noinst_LTLIBRARIES = libggzmod.la
endif

AM_CPPFLAGS = -I$(top_srcdir)/libggz -I$(top_srcdir)/ggzcore

libggzmod_la_LIBADD = $(top_builddir)/ggzcore/libggzcore.la \
	$(top_builddir)/libggz/libggz.la

libggzmod_la_LDFLAGS = -version-info 5:0:1
if SYMBOL_VISIBILITY
libggzmod_la_LDFLAGS += -Wl,--version-script=linker.map
libggzmod_la_DEPENDENCIES = linker.map
endif

libggzmod_la_SOURCES = \
	ggzmod.c \
	ggzmod.h \
	io.c \
	io.h \
	mod.h \
	protocol.h
