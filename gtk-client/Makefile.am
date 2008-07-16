if COND_PIDGIN
PIDGIN_DIR = pidgin-plugin
endif

SUBDIRS = ggz-gtk gtk-client games $(PIDGIN_DIR) man po doc

EXTRA_DIST = TODO README.GGZ QuickStart.GGZ ggz-gtk.desktop

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gtk.desktop

ACLOCAL_AMFLAGS = -I m4 -I m4/ggz
