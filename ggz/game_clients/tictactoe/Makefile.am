## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

bin_PROGRAMS = ttt

ttt_SOURCES = main.c \
	      main_win.c \
	      main_win.h \
	      support.c \
	      support.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ttt_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ttt_LDFLAGS = -static



