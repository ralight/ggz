AM_CPPFLAGS = -I$(top_srcdir)/src/
LDADD = $(top_builddir)/src/libggz.la

check_PROGRAMS = \
	debugmsg \
	dio	\
	cfgtest \
	ggzcommon \
	read_line \
	misc \
	strcmp \
	tls \
	test16 \
	test64 \
	hashing

debugmsg_SOURCES = debugmsg.c
dio_SOURCES = dio.c
cfgtest_SOURCES = cfgtest.c
misc_SOURCES = misc.c
read_line_SOURCES = read_line.c
strcmp_SOURCES = strcmp.c
ggzcommon_SOURCES = ggzcommon.c
tls_SOURCES = tls.c
test16_SOURCES = test16.c
test64_SOURCES = test64.c
hashing_SOURCES = hashing.c

TESTS = debugmsg dio cfgtest ggzcommon read_line misc strcmp tls test16 test64 hashing

clean-local:
	-rm -f debug.log
