
SUBDIRS = 

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)
#LDFLAGS = -static
LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_PROGRAMS =       \
	basic \
	ggzdtest \
	logmod

basic_SOURCES = basic.c
ggzdtest_SOURCES = ggzdtest.c
logmod_SOURCES = logmod.c
