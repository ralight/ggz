## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5339 2003-01-22 13:48:12Z dr_maux $

INCLUDES = -I$(top_srcdir)/src/
LDADD = $(top_builddir)/src/libggz.la

noinst_PROGRAMS =	\
	debugmsg	\
	cfgtest		\
	ggzcommon	\
	read_line	\
	misc		\
	strcmp \
	tls

debugmsg_SOURCES = debugmsg.c
cfgtest_SOURCES = cfgtest.c
misc_SOURCES = misc.c
read_line_SOURCES = read_line.c
strcmp_SOURCES = strcmp.c
ggzcommon_SOURCES = ggzcommon.c
tls_SOURCES = tls.c
#tls_LDADD = $(top_builddir)/src/libggz.la -lrt

TESTS = debugmsg cfgtest ggzcommon read_line misc strcmp tls

clean-local:
	-rm -f debug.log
