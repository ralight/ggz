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

kdots_client_LDADD = $(all_libraries) -lqt -lkdeui -lkdecore -L Dot -ldot $(LIB_EASYSOCK)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in


install-data-local:
	@$(NORMAL_INSTALL)
	#$(GGZ_CONFIG) -D --install --fromfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kdots
	-cp -r *.png $(DESTDIR)$(ggzdatadir)/kdots/
 
uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/kdots/

