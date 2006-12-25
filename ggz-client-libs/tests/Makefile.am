check_PROGRAMS = ggzmodtest ggzmodgametest
TESTS = ggzmodtest

ggzmodtest_SOURCES = ggzmodtest.c
ggzmodtest_CPPFLAGS = -I$(top_srcdir)/ggzmod-ggz
ggzmodtest_LDADD = $(top_builddir)/ggzmod-ggz/libggzmod-ggz.la $(LIB_GGZ)

ggzmodgametest_SOURCES = ggzmodgametest.c
ggzmodgametest_CPPFLAGS = -I$(top_srcdir)/ggzmod
ggzmodgametest_LDADD = $(top_builddir)/ggzmod/libggzmod.la $(LIB_GGZ)
