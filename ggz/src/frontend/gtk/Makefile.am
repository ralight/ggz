
SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/src/ggzcore -I$(top_srcdir)/intl @EASYSOCK_INC@

bin_PROGRAMS = ggz-gtk

ggz_gtk_SOURCES =	main.c \
			about.c \
			about.h \
			chat.c \
			chat.h \
			client.c \
			client.h \
			ggz.c \
			ggz.h \
			license.c \
			license.h \
			login.c \
			login.h \
			xtext.c \
			xtext.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/src/ggzcore/libggzcore.a @LIBS@ @EASYSOCK_LIB@
