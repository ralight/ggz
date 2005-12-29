SUBDIRS = src pics

KDE_ICON = keepalive

desktopdir = $(datadir)/applications
desktop_DATA = keepalive.desktop

EXTRA_DIST = \
	hi32-app-keepalive.png

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

