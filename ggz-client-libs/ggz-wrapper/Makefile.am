bin_PROGRAMS = ggz-wrapper
dist_bin_SCRIPTS = ggz

noinst_HEADERS = server.h loop.h game.h

ggz_wrapper_SOURCES = ggz-wrapper.c server.c loop.c game.c
ggz_wrapper_CPPFLAGS = -I$(top_srcdir)/ggzcore/ $(LIBGGZ_INCLUDES)
ggz_wrapper_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggz_wrapper_LDADD = $(top_builddir)/ggzcore/libggzcore.la $(top_builddir)/ggzmod/libggzmod.la $(LIB_GGZ) $(LIB_GETTEXT)

EXTRA_DIST = README.wrapper
