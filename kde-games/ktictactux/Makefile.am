SUBDIRS =

bindir = $(ggzexecmoddir)

bin_PROGRAMS = ktictactux_client

ktictactux_client_SOURCES = \
	main.cpp \
	ktictactux.cpp \
	ktictactuxproto.cpp \
	qwhiteframe.cpp

noinst_HEADERS = \
	ktictactux.h \
	ktictactuxproto.h \
	qwhiteframe.h

INCLUDES = $(EASYSOCK_INCLUDES) $(all_includes)

UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
ktictactux_client_LDFLAGS = -rpath $(easysock_libraries)
ktictactux_client_LDADD = $(all_libraries) $(UI) $(EASYSOCK_LDFLAGS) $(LIB_EASYSOCK)

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --fromfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/ktictactux
	-$(INSTALL) -m 644 *.png $(DESTDIR)$(ggzdatadir)/ktictactux/

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --fromfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/ktictactux

