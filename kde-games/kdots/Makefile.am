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

INCLUDES = $(all_includes) $(EASYSOCK_INCLUDES) -I Dot

# A note on LDADD: on my machine, the -l is prepended automatically to
# everything listed in the LDADD section.  This means putting
# "-L Dot -ldot" didn't have good results.  I've changed this to just
# "Dot/libdot.a", which works fine for me (and should for everyone else
# too).  --JDS

#UI = -L$(qt_libraries) -L$(kde_libraries) -lqt -lkdeui -lkdecore
#kdots_client_LDFLAGS = -rpath $(easysock_libraries)
kdots_client_LDFLAGS = $(all_libraries) $(EASYSOCK_LDFLAGS)
kdots_client_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_EASYSOCK) Dot/libdot.a

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

