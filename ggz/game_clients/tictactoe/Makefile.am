## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

bin_PROGRAMS = ttt

ttt_SOURCES = main.c

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ttt_LDADD = @GTK_LIBS@ $(LDADD)


