SUBDIRS = modules

kde_module_LTLIBRARIES = libkcm_ggz.la
libkcm_ggz_la_SOURCES = kcm_ggz.cpp
libkcm_ggz_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkcm_ggz_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)

noinst_PROGRAMS = kcm_ggz_test
kcm_ggz_test_SOURCES = main.cpp
kcm_ggz_test_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
kcm_ggz_test_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT) libkcm_ggz.la

AM_CPPFLAGS = $(all_includes)

noinst_HEADERS = kcm_ggz.h kcm_ggz_pane.h

METASOURCES = AUTO

desktopdir = $(datadir)/applications
dist_desktop_DATA = kcmggz.desktop

