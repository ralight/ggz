## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3766 2002-04-06 07:43:25Z jdorje $

SUBDIRS = .

# FIXME: should these both be distributed?
EXTRA_DIST = README.minidom metasrv.xml

noinst_LIBRARIES = libminidom.a
libminidom_a_SOURCES = minidom.c minidom.h

bin_PROGRAMS = minidom
minidom_SOURCES = main.c
minidom_LDADD = libminidom.a

#all:
#	gcc -g -Wall -c minidom.c
#	gcc -g -o minidom minidom.o
#
#clean:
#	rm -f minidom
#	rm -f *.o

