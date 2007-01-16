EXTRA_DIST = Doxyfile

if INSTALL_GGZMOD
lib_LTLIBRARIES = libggzmod.la
include_HEADERS = ggzmod.h
else
noinst_LTLIBRARIES = libggzmod.la
endif

libggzmod_la_CPPFLAGS = $(LIBGGZ_INCLUDES) -I$(top_srcdir)/ggzcore

libggzmod_la_LIBADD = $(top_builddir)/ggzcore/libggzcore.la
if INSTALL_GGZMOD
libggzmod_la_LIBADD += $(LIB_GGZ)
endif

libggzmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 5:0:1

libggzmod_la_SOURCES = \
	ggzmod.c \
	ggzmod.h \
	io.c \
	io.h \
	mod.h \
	protocol.h
