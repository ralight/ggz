SUBDIRS = src pics

KDE_ICON = keepalive

desktopdir = $(datadir)/applications
desktop_DATA = keepalive.desktop

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

