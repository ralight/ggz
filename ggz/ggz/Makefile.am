## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock

bin_PROGRAMS = ggz

ggz_SOURCES = 	main.c \
		callbacks.c \
	   	callbacks.h \
		connect.c \
	   	connect.h \
		datatypes.h \
		dlg_error.c \
		dlg_error.h \
		dlg_exit.c \
		dlg_exit.h \
		err_func.c \
		err_func.h \
		game.c \
		game.h \
		interface.c \
		interface.h \
		parse_opt.c \
		parse_opt.h \
		protocols.h \
		support.c \
	 	support.h


CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggz_LDFLAGS = -static

