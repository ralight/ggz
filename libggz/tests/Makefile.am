## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4596 2002-09-17 02:32:26Z jdorje $

INCLUDES = -I$(top_srcdir)/src/
LDADD = $(top_builddir)/src/libggz.la

noinst_PROGRAMS =	\
	debugmsg	\
	cfgtest		\
	ggzcommon	\
	read_line	\
	misc		\
	strcmp

debugmsg_SOURCES = debugmsg.c
cfgtest_SOURCES = cfgtest.c
misc_SOURCES = misc.c
read_line_SOURCES = read_line.c
strcmp_SOURCES = strcmp.c
ggzcommon_SOURCES = ggzcommon.c

TESTS = debugmsg cfgtest ggzcommon read_line misc strcmp

clean-local:
	-rm -f debug.log
