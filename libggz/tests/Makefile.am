INCLUDES = -I$(top_srcdir)/src/
LDADD = $(top_builddir)/src/libggz.la

noinst_PROGRAMS =	\
	debugmsg	\
	cfgtest		\
	read_line	\
	misc

debugmsg_SOURCES = debugmsg.c
cfgtest_SOURCES = cfgtest.c
misc_SOURCES = misc.c
read_line_SOURCES = read_line.c

TESTS = debugmsg cfgtest misc read_line

