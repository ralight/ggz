SUBDIRS = Dot .

ggzexecmod_PROGRAMS = kdots_client

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

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES) -I Dot

kdots_client_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS)
kdots_client_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ) Dot/libdot.a

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in

KDE_ICON = kdots

install-data-local:
	@$(NORMAL_INSTALL)
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
	-$(mkinstalldirs) $(DESTDIR)$(ggzdatadir)/kdots
	-$(INSTALL) -m 644 *.png $(DESTDIR)$(ggzdatadir)/kdots/
 
uninstall:
	@$(NORMAL_UNINSTALL)
	-$(GGZ_CONFIG) --remove --modfile=module.dsc
	-rm -rf $(DESTDIR)$(ggzdatadir)/kdots/

