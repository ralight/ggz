SUBDIRS = .

bin_PROGRAMS = metaserv

metaserv_SOURCES = metaserv.c
metaserv_LDADD = ../minidom/libminidom.a
INCLUDES = -I../minidom

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

