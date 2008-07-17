bin_PROGRAMS = ggz-config

AM_CPPFLAGS = -I$(top_srcdir)/ggzcore \
	-I$(top_srcdir)/libggz	\
	-DGGZCONFDIR=\"$(sysconfdir)\"					\
	-DGGZDATADIR=\"$(datadir)/ggz\"					\
	-DGGZEXECMODDIR=\"$(libdir)/ggz\"				\
	-DPREFIX=\"$(prefix)\"
ggz_config_LDADD = $(top_builddir)/ggzcore/libggzcore.la \
		$(top_builddir)/libggz/libggz.la $(LIB_GETTEXT)
ggz_config_SOURCES = ggz-config.c

EXTRA_DIST = readme.ggzconfig

