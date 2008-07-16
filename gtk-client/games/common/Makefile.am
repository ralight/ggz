## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 8432 2006-07-31 23:54:12Z jdorje $

SUBDIRS = . cardsets

AM_CPPFLAGS = $(GTK_CFLAGS) $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

noinst_LIBRARIES = libgtk-games.a

libgtk_games_a_SOURCES =	dlg_about.c	dlg_about.h	\
				dlg_chat.c	dlg_chat.h	\
				dlg_exit.c	dlg_exit.h	\
				dlg_players.c	dlg_players.h	\
				ggz_gtk.c	ggz_gtk.h	\
				ggzintl.c	ggzintl.h	\
				menus.c		menus.h
