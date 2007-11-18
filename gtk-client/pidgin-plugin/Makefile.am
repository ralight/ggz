plugindir = $(libdir)/pidgin

plugin_LTLIBRARIES = ggzpidgin.la

ggzpidgin_la_SOURCES = simple.c
ggzpidgin_la_LDFLAGS = -module -avoid-version $(GTK_CFLAGS) $(LIBGGZ_LDFLAGS)
ggzpidgin_la_LIBADD = $(GTK_LIBS) $(LIB_GGZ)

AM_CPPFLAGS = $(GTK_CFLAGS) $(LIBGGZ_INCLUDES) -I /usr/include/libpurple \
	-DPREFIX=\"$(prefix)\"

EXTRA_DIST = ggz.xpm

