SUBDIRS = . modules

QUICK1 = kcm_ggz.moc.lo
QUICK2 = kcm_ggz_pane.moc.lo
KSTUFF = $(all_libraries) $(KDE_PLUGIN) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)

lib_LTLIBRARIES = libkcm_ggz.la
libkcm_ggz_la_SOURCES = kcm_ggz.cpp
libkcm_ggz_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)

bin_PROGRAMS = kcm_ggz_test
kcm_ggz_test_SOURCES = main.cpp kcm_ggz.cpp
kcm_ggz_test_LDFLAGS = $(QUICK1) $(QUICK2)

INCLUDES = -I $(qt_includes) -I $(kde_includes)
LDFLAGS = $(KSTUFF)

noinst_HEADERS = kcm_ggz.h kcm_ggz_pane.h

METASOURCES = AUTO

data_DATA = kcmggz.desktop
datadir = $(kde_appsdir)/Settings/Network

