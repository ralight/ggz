METASOURCES = AUTO

AM_CPPFLAGS = $(KOPETE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = kopete_ggz.la

kopete_ggz_la_SOURCES     = ggz.cpp
kopete_ggz_la_LDFLAGS     = -module -no-undefined $(KDE_PLUGIN)
kopete_ggz_la_LIBADD      = ../../libkopete/libkopete.la

service_DATA = kopete_ggz.desktop
servicedir = $(kde_servicesdir)

mydatadir = $(kde_datadir)/kopete_ggz
mydata_DATA = ggzchatui.rc

