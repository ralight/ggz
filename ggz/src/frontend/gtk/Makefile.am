
SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/intl

bin_PROGRAMS = ggz-gtk

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
			ggz.c \
			ggz.h \
			info.c \
			info.h \
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
			props.c \
			props.h \
			server.c \
			server.h \
			support.c \
			support.h \
			types.c \
			types.h \
			xtext.c \
			xtext.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) @LIBS@
