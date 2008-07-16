noinst_LTLIBRARIES = libggzmod-ggz.la

libggzmod_ggz_la_SOURCES = \
	ggzmod-ggz.c \
	ggzmod-ggz.h \
	io-ggz.c \
	io-ggz.h \
	mod-ggz.h \
	protocol.h

libggzmod_ggz_la_LIBADD = $(top_builddir)/libggz/libggz.la

AM_CPPFLAGS = $(LIBGGZ_INCLUDES) -I$(top_srcdir)/libggz/
