bin_PROGRAMS = ggz-wrapper
dist_bin_SCRIPTS = ggz

noinst_HEADERS = server.h loop.h game.h

ggz_wrapper_SOURCES = ggz-wrapper.c server.c loop.c game.c
ggz_wrapper_CPPFLAGS = $(GGZCORE_INCLUDES) $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)
ggz_wrapper_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZCORE_LDFLAGS) $(GGZMOD_LDFLAGS)
ggz_wrapper_LDADD = $(LIB_GGZCORE) $(LIB_GGZMOD) $(LIB_GGZ) $(LIB_GETTEXT)

EXTRA_DIST = README.wrapper
