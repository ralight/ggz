SUBDIRS = .

KDE_ICON = kggz ggz ggzmodsniff ggzquick ggzap

apps_DATA = kggz.desktop ggzmodsniff.desktop
appsdir = $(kde_appsdir)/Games/ggz

install-data-local:
	@$(NORMAL_INSTALL)
	-cp dist.directory $(DESTDIR)$(kde_appsdir)/Games/ggz/.directory
	$(mkinstalldirs) $(DESTDIR)$(kde_appsdir)/Games/ggz/games
	-cp games.directory $(DESTDIR)$(kde_appsdir)/Games/ggz/games/.directory

