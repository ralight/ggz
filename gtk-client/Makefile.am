if COND_GAIM
GAIM_DIR = gaim-plugin
endif

SUBDIRS = ggz-gtk src $(COND_GAIM) man po doc
DIST_SUBDIRS = ggz-gtk gaim-plugin man po doc

EXTRA_DIST = TODO README.GGZ QuickStart.GGZ ggz-gtk.desktop \
		m4/gettext.m4	\
		m4/ggz.m4	\
		m4/ggz-debug.m4	\
		m4/glibc21.m4	\
		m4/gtk-2.0.m4	\
		m4/iconv.m4	\
		m4/lcmessage.m4	\
		m4/locale.m4	\
		m4/progtest.m4	\
		m4/gaim.m4	\
		m4/system.m4

desktopdir = $(datadir)/applications
desktop_DATA = ggz-gtk.desktop
