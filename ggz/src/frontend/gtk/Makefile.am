## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6255 2004-11-04 22:38:30Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

bin_PROGRAMS = ggz-gtk

ggzdata_DATA =		about_bg.png		\
			ggz_gtk_admin.png 	\
			ggz_gtk_guest.png 	\
			ggz_gtk_registered.png	\
			ggz_gtk_lag0.png 	\
			ggz_gtk_lag1.png 	\
			ggz_gtk_lag2.png	\
			ggz_gtk_lag3.png	\
			ggz_gtk_lag4.png	\
			ggz_gtk_lag5.png	\
			msg_help.png		\
			msg_info.png		\
			msg_stop.png

ggz_gtk_SOURCES =	main.c \
			about.c \
			about.h \
			chat.c \
			chat.h \
			client.c \
			client.h \
			first.c \
			first.h \
			game.c \
			game.h \
			ggzclient.c \
			ggzclient.h \
			launch.c \
			launch.h \
			license.c \
			license.h \
			login.c \
			login.h \
			motd.c \
			motd.h \
			msgbox.c \
			msgbox.h \
			pick_module.c \
			pick_module.h \
			playerinfo.c \
			playerinfo.h \
			playerlist.c \
			playerlist.h \
			props.c \
			props.h \
			roominfo.c \
			roominfo.h \
			server.c \
			server.h \
			support.c \
			support.h \
			tablelist.c \
			tablelist.h \
			types.c \
			types.h \
			xtext.c \
			xtext.h

ggz_gtk_CFLAGS = @GTK_CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) @LIBS@ @INTLLIBS@ \
		$(LIB_GGZCORE) $(LIB_GGZMOD) $(LIB_GGZ)
ggz_gtk_LDFLAGS = $(GGZCORE_LDFLAGS) $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

EXTRA_DIST = $(ggzdata_DATA)
