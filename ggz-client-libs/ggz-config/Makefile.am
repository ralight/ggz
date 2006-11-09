bin_PROGRAMS = ggz-config

ggz_config_CPPFLAGS = -I$(top_srcdir)/ggzcore $(LIBGGZ_INCLUDES)	\
	-DGGZCONFDIR=\"$(sysconfdir)\"					\
	-DGGZDATADIR=\"$(datadir)/ggz\"					\
	-DGGZEXECMODDIR=\"$(libdir)/ggz\"				\
	-DPREFIX=\"$(prefix)\"
ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la $(LIB_GGZ) $(LIB_GETTEXT)
ggz_config_LDFLAGS = $(LIBGGZ_LDFLAGS)
ggz_config_SOURCES = ggz-config.c

EXTRA_DIST = readme.ggzconfig

