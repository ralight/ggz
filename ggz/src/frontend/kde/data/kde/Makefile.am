SUBDIRS = .

KDE_ICON = kggz ggz ggzmodsniff ggzquick ggzap

apps_DATA = kggz.desktop ggzmodsniff.desktop
appsdir = $(kde_appsdir)/Games/ggz

install-data-local:
	@$(NORMAL_INSTALL)
	-cp dist.directory $(kde_appsdir)/Games/ggz/.directory
	$(mkinstalldirs) $(kde_appsdir)/Games/ggz/games
	-cp games.directory $(kde_appsdir)/Games/ggz/games/.directory
	#-cp modsniff.desktop $(kde_appsdir)/Games/ggz/games/

