SUBDIRS = .

bin_PROGRAMS = metaserv ggzmetaserv

metaserv_SOURCES = metaserv.c
metaserv_LDADD = ../minidom/libminidom.a

ggzmetaserv_SOURCES = server.c gensocket.c
ggzmetaserv_LDFLAGS = -L/usr/local/lib
ggzmetaserv_LDADD = -lgensecure

INCLUDES = -I../minidom -I/usr/local/ssl/include -I/usr/local/include

confdir = $(prefix)/share/metaserv
conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv metaservconf.xml

#all:
#	gcc -Wall -c metaserv.c
#	gcc -o metaserv metaserv.o -lggzcore
#
#clean:
#	rm -f *.o
#	rm -f metaserv

