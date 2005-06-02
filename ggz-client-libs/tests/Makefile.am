## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 7265 2005-06-02 15:02:10Z josef $

AM_CPPFLAGS = -I$(top_srcdir)/ggzmod -I$(top_srcdir)/ggzmod-ggz
#LDADD = $(top_builddir)/ggzmod/libggzmod.la @LDADD@

check_PROGRAMS = ggzmodtest ggzmodgametest
TESTS = ggzmodtest

ggzmodtest_SOURCES = ggzmodtest.c
ggzmodtest_LDADD = $(top_builddir)/ggzmod-ggz/libggzmod-ggz.la

ggzmodgametest_SOURCES = ggzmodgametest.c
ggzmodgametest_LDADD = $(top_builddir)/ggzmod/libggzmod.la
