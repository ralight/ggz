## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 8816 2007-01-04 23:31:54Z jdorje $

DIST_SUBDIRS = ggzdmod ggzdmod++ ggzd game_servers man tests
CLEAN_FILES = ggzd.debug ggzd.log
SUBDIRS = $(COMPILE_SUBDIRS)

EXTRA_DIST = README.GGZ \
		m4/test.mk

include $(top_srcdir)/m4/test.mk

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
