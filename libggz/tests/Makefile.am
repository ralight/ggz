AM_CPPFLAGS = -I$(top_srcdir)/src/
LDADD = $(top_builddir)/src/libggz.la

check_PROGRAMS = \
	debugmsg \
	cfgtest \
	ggzcommon \
	read_line \
	misc \
	strcmp \
	tls

debugmsg_SOURCES = debugmsg.c
cfgtest_SOURCES = cfgtest.c
misc_SOURCES = misc.c
read_line_SOURCES = read_line.c
strcmp_SOURCES = strcmp.c
ggzcommon_SOURCES = ggzcommon.c
tls_SOURCES = tls.c

TESTS = debugmsg cfgtest ggzcommon read_line misc strcmp tls

clean-local:
	-rm -f debug.log
