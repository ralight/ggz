#dist-hook:
#	-perl am_edit

SUBDIRS = zoneclient gfx

bin_PROGRAMS = krosswater_client

bindir = $(ggzexecmoddir)

INCLUDES = -Izoneclient -I$(qt_includes) -I$(kde_includes) $(all_includes) $(EASYSOCK_INCLUDES)

noinst_HEADERS = krosswater.h \
		dlg_person.h \
		dlg_about.h \
		dlg_help.h \
		dlg_again.h \
		qcw.h

krosswater_client_SOURCES = main.cpp \
			krosswater.cpp \
			dlg_person.cpp \
			dlg_about.cpp \
			dlg_help.cpp \
			dlg_again.cpp \
			qcw.cpp

krosswater_client_METASOURCES = AUTO
#krosswater_client_METASOURCES = krosswater.moc.cpp dlg_person.moc.cpp qcw.moc.cpp

EXTRA_DIST =

UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
krosswater_client_LDADD = zoneclient/libzoneclient.la $(UI) $(EASYSOCK_LDFLAGS) $(LIB_EASYSOCK)

DISTCLEANFILES = $(krosswater_client_METASOURCES)

# AUTOMAKE_OPTIONS = foreign

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --fromfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/krosswater/gfx
	-$(INSTALL) -m 644 gfx/*.png $(DESTDIR)$(ggzdatadir)/krosswater/gfx/

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/krosswater/

