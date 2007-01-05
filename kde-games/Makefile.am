## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8829 2007-01-05 00:34:33Z jdorje $

SUBDIRS = lib krosswater KReversi kdots ktictactux koenig muehle keepalive fyrdman kcc man po kconnectx

EXTRA_DIST = README.GGZ QuickStart.GGZ


dist-hook:
	-perl am_edit --foreign-libtool --no-autodist

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
