## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5836 2004-02-07 23:50:18Z jdorje $

SUBDIRS = . cardsets

INCLUDES = @GTK_CFLAGS@ $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

noinst_LIBRARIES = libgtk-games.a

libgtk_games_a_SOURCES =	dlg_about.c	dlg_about.h	\
				dlg_exit.c	dlg_exit.h	\
				dlg_players.c	dlg_players.h	\
				ggzintl.c	ggzintl.h	\
				menus.c		menus.h
