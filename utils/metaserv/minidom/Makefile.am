## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3825 2002-04-07 10:47:10Z dr_maux $

SUBDIRS = .

# FIXME: should these both be distributed?
EXTRA_DIST = README.minidom metasrv.xml

noinst_LIBRARIES = libminidom.a
libminidom_a_SOURCES = minidom.c minidom.h

noinst_PROGRAMS = minidom
minidom_SOURCES = main.c
minidom_LDADD = libminidom.a

#all:
#	gcc -g -Wall -c minidom.c
#	gcc -g -o minidom minidom.o
#
#clean:
#	rm -f minidom
#	rm -f *.o

