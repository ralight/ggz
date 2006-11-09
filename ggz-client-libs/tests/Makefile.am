check_PROGRAMS = ggzmodtest ggzmodgametest
TESTS = ggzmodtest

ggzmodtest_SOURCES = ggzmodtest.c
AM_CPPFLAGS = -I$(top_srcdir)/ggzmod-ggz
ggzmodtest_LDADD = $(top_builddir)/ggzmod-ggz/libggzmod-ggz.la

ggzmodgametest_SOURCES = ggzmodgametest.c
AM_CPPFLAGS = -I$(top_srcdir)/ggzmod
ggzmodgametest_LDADD = $(top_builddir)/ggzmod/libggzmod.la
