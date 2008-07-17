noinst_LTLIBRARIES = libggzsecurity.la

libggzsecurity_la_SOURCES = \
	hashfunc.c \
	base16.c \
	base64.c \
	ggz_tls_none.c

if GGZ_TLS_GNUTLS
libggzsecurity_la_SOURCES += ggz_tls_gnutls.c
endif
if GGZ_TLS_OPENSSL
libggzsecurity_la_SOURCES +=ggz_tls_openssl.c
endif
if GGZ_TLS_NSS
libggzsecurity_la_SOURCES += ggz_tls_nss.c
endif

libggzsecurity_la_LDFLAGS = $(GGZTLS_LDFLAGS)
libggzsecurity_la_LIBADD = $(LIB_GGZTLS) $(LIB_GCRYPT)

libggzsecurity_la_CPPFLAGS = -I$(srcdir)/.. $(GGZTLS_INCLUDES)

