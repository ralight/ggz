plugindir = $(libdir)/gaim

plugin_LTLIBRARIES = ggzgaim.la

ggzgaim_la_SOURCES = simple.c
ggzgaim_la_LDFLAGS = -module -avoid-version @GTK_CFLAGS@ $(LIBGGZ_LDFLAGS)
ggzgaim_la_LIBADD = @GTK_LIBS@ $(LIB_GGZ)

INCLUDES = @GTK_CFLAGS@ $(LIBGGZ_INCLUDES)

EXTRA_DIST = ggz.xpm

