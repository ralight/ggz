AM_CPPFLAGS = -I$(top_srcdir)/libggz/ -I$(top_srcdir)/ggzmod -I$(top_srcdir)/ggzmod-ggz
LDADD = $(top_builddir)/libggz/libggz.la \
	$(top_builddir)/ggzmod/libggzmod.la \
	$(top_builddir)/ggzmod-ggz/libggzmod-ggz.la

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
	hashing \
	ggzmodtest \
	ggzmodgametest

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
ggzmodtest_SOURCES = ggzmodtest.c
ggzmodgametest_SOURCES = ggzmodgametest.c

TESTS = debugmsg dio cfgtest ggzcommon read_line misc strcmp tls test16 test64 hashing

clean-local:
	-rm -f debug.log
