SUBDIRS = .

noinst_LTLIBRARIES = libggzsecurity.la

libggzsecurity_la_SOURCES = \
	hashfunc.c hashfunc.h \
	base64.c base64.h \
	ggz_tls_gnutls.c \
	ggz_tls_openssl.c \
	ggz_tls_none.c

AM_CPPFLAGS = -I $(top_srcdir)/src

# from tls-branch
#
#lib_LTLIBRARIES = libggztls.la
#
#libggztls_la_SOURCES = ggz_tls_openssl.c ggz_tls_gnutls.c ggz_tls_none.c
#libggztls_la_LIBADD = -L $(GGZTLS_LDFLAGS) $(LIB_GGZTLS)
#
#AM_CPPFLAGS = -I $(top_srcdir) -I $(top_srcdir)/src
#
#noinst_HEADERS = list.h
#include_HEADERS = ggz_tls.h

