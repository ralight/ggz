## Process this file with automake to produce Makefile.in

DIST_SUBDIRS = ggzdmod ggzd game_servers man tests
CLEAN_FILES = ggzd.debug ggzd.log
SUBDIRS = $(COMPILE_SUBDIRS)

EXTRA_DIST = HACKING README.GGZ \
		m4/c99.m4 m4/ggz.m4 m4/ggz-debug.m4 m4/iconv.m4 m4/test.mk

include $(top_srcdir)/m4/test.mk

