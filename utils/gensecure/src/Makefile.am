SUBDIRS = .

bin_PROGRAMS = genserver genclient
lib_LTLIBRARIES = libgensecure.la

libgensecure_la_SOURCES = tls.c

INCLUDES = -I/usr/local/ssl/include
LDFLAGS = -L/usr/local/ssl/lib
libgensecure_la_LIBADD = -lssl

genclient_SOURCES = client.c gensocket.c
genclient_LDADD = libgensecure.la

genserver_SOURCES = server.c gensocket.c
genserver_LDADD = libgensecure.la

include_HEADERS = gensecure.h
noinst_HEADERS = tls.h

