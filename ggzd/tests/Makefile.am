
AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
#LDFLAGS = -static
LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

# logmod is a helper program for ggzdtest, but it's broken
TESTS = basic # ggzdtest
check_PROGRAMS = $(TESTS) #logmod

basic_SOURCES = basic.c
#ggzdtest_SOURCES = ggzdtest.c
#logmod_SOURCES = logmod.c
