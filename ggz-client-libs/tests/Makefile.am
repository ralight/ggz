## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6176 2004-09-04 07:22:54Z jdorje $

AM_CPPFLAGS = -I$(top_srcdir)/ggzmod
LDADD = $(top_builddir)/ggzmod/libggzmod.la @LDADD@

check_PROGRAMS = ggzmodtest ggzmodgametest
TESTS = ggzmodtest

ggzmodtest_SOURCES = ggzmodtest.c
#ggzmodtest_LDADD = $(top_builddir)/ggzmod/libggzmod.la

ggzmodgametest_SOURCES = ggzmodgametest.c
#ggzmodgametest_LDADD = $(top_builddir)/ggzmod/libggzmod.la
