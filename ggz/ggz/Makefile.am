## Process this file with automake to produce Makefile.in

EXTRA_DIST = ggzrc.test.in

SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/easysock -I$(top_srcdir)/intl

bin_PROGRAMS = ggz

ggz_SOURCES = 	main.c \
		chat.c \
	   	chat.h \
		connect.c \
	   	connect.h \
		datatypes.h \
		dlg_about.c \
		dlg_about.h \
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
		dlg_msgbox.h \
		dlg_msgbox.c \
		dlg_main.h \
		dlg_motd.c \
		dlg_motd.h \
		dlg_props.c \
		dlg_props.h \
		err_func.c \
		err_func.h \
		game.c \
		game.h \
		ggzrc.c \
		ggzrc.h \
		mnu_players.c\
		mnu_players.h\
		mnu_tables.c\
		mnu_tables.h\
		parse_opt.c \
		parse_opt.h \
		player.c \
		player.h \
		protocols.h \
		seats.c \
		seats.h \
		support.c \
	 	support.h \
		table.c \
		table.h \
		xtext.c \
		xtext.h


CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/easysock/libeasysock.la @LIBS@
ggz_LDFLAGS = -static

