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
		dlg_details.c \
		dlg_details.h \
		dlg_error.c \
		dlg_error.h \
		dlg_exit.c \
		dlg_exit.h \
		dlg_launch.c \
		dlg_launch.h \
		dlg_login.c \
		dlg_login.h \
		dlg_main.c \
		dlg_main.h \
		err_func.c \
		err_func.h \
		game.c \
		game.h \
		parse_opt.c \
		parse_opt.h \
		protocols.h \
		seats.c \
		seats.h \
		support.c \
	 	support.h


CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggz_LDFLAGS = -static

