METASOURCES = AUTO

AM_CPPFLAGS = $(KOPETE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = kopete_ggz.la kcm_kopete_ggz.la

kopete_ggz_la_SOURCES     = ggz.cpp
kopete_ggz_la_LDFLAGS     = -module -no-undefined $(KDE_PLUGIN)
kopete_ggz_la_LIBADD      = ../../libkopete/libkopete.la

kcm_kopete_ggz_la_SOURCES = plugin_kcm.cpp
kcm_kopete_ggz_la_LDFLAGS = -module -no-undefined $(KDE_PLUGIN) $(all_libraries)
kcm_kopete_ggz_la_LIBADD = $(LIB_KUTILS)

service_DATA = kopete_ggz.desktop
servicedir = $(kde_servicesdir)

kcmdir = $(kde_servicesdir)/kconfiguredialog
kcm_DATA = kopete_ggz_config.desktop

mydatadir = $(kde_datadir)/kopete_ggz
mydata_DATA = ggzchatui.rc

