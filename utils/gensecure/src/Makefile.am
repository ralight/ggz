SUBDIRS = .

bin_PROGRAMS = genserver genclient

CFLAGS = -I/usr/local/ssl/include
LDFLAGS = -L/usr/local/ssl/lib -lssl
LDADD =

genclient_SOURCES = client.c gensocket.c tls.c

genserver_SOURCES = server.c gensocket.c tls.c

