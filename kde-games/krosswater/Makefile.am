SUBDIRS = zoneclient gfx

ggzexecmod_PROGRAMS = krosswater_client

krosswater_client_SOURCES = \
	main.cpp \
	krosswater.cpp \
	dlg_person.cpp \
	dlg_about.cpp \
	dlg_help.cpp \
	dlg_again.cpp \
	qcw.cpp

krosswater_client_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS)
krosswater_client_LDADD = -lqt $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_GGZ) zoneclient/libzoneclient.la

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) -Izoneclient

noinst_HEADERS = \
	krosswater.h \
	dlg_person.h \
	dlg_about.h \
	dlg_help.h \
	dlg_again.h \
	qcw.h

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

DISTCLEANFILES = $(METASOURCES)

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/krosswater/gfx
	-$(INSTALL) -m 644 gfx/*.png $(DESTDIR)$(ggzdatadir)/krosswater/gfx/

uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/krosswater/

