## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/easysock 

bin_PROGRAMS = tictactoe

tictactoe_SOURCES = ggz.c \
		    ggz.h \
		    main.c

tictactoe_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
tictactoe_LDFLAGS = -static
