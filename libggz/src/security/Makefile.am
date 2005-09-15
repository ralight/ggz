noinst_LTLIBRARIES = libggzsecurity.la

libggzsecurity_la_SOURCES = \
	hashfunc.c hashfunc.h \
	base64.c base64.h \
	ggz_tls_gnutls.c \
	ggz_tls_openssl.c \
	ggz_tls_none.c

libggzsecurity_la_LDFLAGS = $(GGZTLS_LDFLAGS)
libggzsecurity_la_LIBADD = $(LIB_GGZTLS)

libggzsecurity_la_CPPFLAGS = -I $(top_srcdir)/src $(GGZTLS_INCLUDES)

