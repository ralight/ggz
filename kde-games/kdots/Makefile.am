SUBDIRS = Dot .

bindir = $(ggzexecmoddir)

bin_PROGRAMS = kdots_client

kdots_client_SOURCES = \
	main.cpp \
	kdots.cpp \
	kdots_about.cpp \
	kdots_options.cpp \
	kdots_help.cpp \
	kdots_proto.cpp \
	kdots_win.cpp

noinst_HEADERS = \
	kdots.h \
	kdots_about.h \
	kdots_options.h \
	kdots_help.h \
	kdots_proto.h \
	kdots_win.h

INCLUDES = $(EASYSOCK_INCLUDES) $(all_includes) -I Dot

UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
kdots_client_LDFLAGS = -rpath $(easysock_libraries)
kdots_client_LDADD = $(all_libraries) $(UI) -L Dot -ldot $(EASYSOCK_LDFLAGS) $(LIB_EASYSOCK)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in


install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kdots
	-$(INSTALL) -m 644 *.png $(DESTDIR)$(ggzdatadir)/kdots/
 
uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/kdots/

