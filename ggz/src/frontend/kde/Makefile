# Makefile for KGGZ
# (C) 2000 Josef Spillner

# Revision 1.1: Taken from mindx birthday demo (2000-10-??) (josef)
# Revision 1.5: Changed structure to fit to CVS,
# translated comments, release + install + uninstall option (2000-10-29) (josef)
# Revision 1.5.1: Added Desktop icon/applink for KDE2 (2000-10-29) (josef)
# Revision 1.5.2: Added kicker entry and first i18n for icons (2000-11-04) (josef)
# Revision 1.5.3: RedHat FSH support (2000-11-17) (josef)


# The important dirs - this seems to become autoconf stuff
# KDEDIR=/usr/local/kde - this is obsolete, don't use it anymore
# QTDIR=/usr/local/qt - same
KDELIBS=/usr/local/kde/lib
KDEINCS=/usr/local/kde/include
QTLIBS=/usr/local/qt/lib
QTINCS=/usr/local/qt/include
TARGETDIR=/usr/local

#################################################################################

# Internal variables
QUIET=@
PROGRESS=-n "\#"
SHELL=/bin/sh
CXX=g++
CPP_TEMP=-I../../ggzcore
LD_TEMP=../../ggzcore/libggzcore.a ../../../easysock/libeasysock.a
CPPFLAGS=-c -I$(KDEINCS) -I$(QTINCS) $(CPP_TEMP) -fno-rtti
LDFLAGS=-L$(KDELIBS) -L$(QTLIBS)
LIBS=-lkdeui -lkdecore -lqt -ldl -lDCOP
DESTDIR=$(TARGETDIR)
VERSION=0.0.2


# List of sources
SOURCES=kggz kggz_roomsmenu \
	kggz_state kggz_chat kggz_startup kggz_tables kggz_users \
	kggz_chatwidget kggz_motd \
	kggz_preferences kggz_settings kggz_profiles kggz_connect kggz_launch \
	kggz_server


# Rules
default: all
all: compile compile_moc icon link
uninstall: clean distclean
release: clean_tmp package unclean_tmp upload


# Building KGGZ in 3 steps, plus icon, plus install, or clean up
compile:
	$(QUIET) echo KGGZ
	$(QUIET) echo The KDE client for GNU Gaming Zone
	$(QUIET) echo Compilation of the source files:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		$(CXX) $(CPPFLAGS) $$i.cpp; \
		echo $(PROGRESS); \
	done
	$(QUIET) $(CXX) $(CPPFLAGS) main.cpp;
	$(QUIET) echo $(PROGRESS);
	$(QUIET) echo

compile_moc:
	$(QUIET) echo Meta objects compiler invokation:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		$(QTDIR)/bin/moc -nw -o $$i.moc.cpp $$i.h; \
		$(CXX) $(CPPFLAGS) $$i.moc.cpp; \
		echo $(PROGRESS); \
	done; \
	echo;

icon:
	$(QUIET) cp images/kggz.png $(HOME)/.kde/share/icons/
	$(QUIET) cp images/kggz_small.png $(HOME)/.kde/share/icons/
	$(QUIET) echo $(HOME) > .user.homedir
	$(QUIET) echo `whoami` > .user.user
	$(QUIET) echo `groups | cut -d " " -f 1`  > .user.group

link:
	$(QUIET) echo Link stage:
	$(QUIET) SOURCELIST='$(SOURCES)'; \
	for i in $$SOURCELIST; do \
		OBJLIST=`echo -n $$OBJLIST $$i.o" "`; \
		MOCOBJLIST=`echo -n $$MOCOBJLIST $$i.moc.o" "`; \
        done; \
	$(CXX) $(LDFLAGS) $(LIBS) -o kggz main.o $$OBJLIST $$MOCOBJLIST $(LD_TEMP)
	$(QUIET) echo Ready. Now run "'make install'" as root!

install:
	$(QUIET) echo Installing KGGZ
	$(QUIET) install -m 755 kggz $(DESTDIR)/bin
	$(QUIET) rm -rf $(DESTDIR)/share/kggz/
	$(QUIET) mkdir --mode=755 $(DESTDIR)/share/kggz/
	$(QUIET) mkdir --mode=755 $(DESTDIR)/share/kggz/images/
	$(QUIET) mkdir --mode=755 $(DESTDIR)/share/kggz/help/
	$(QUIET) install -m 644 images/*.png $(DESTDIR)/share/kggz/images/
	$(QUIET) install -m 644 help/*.html $(DESTDIR)/share/kggz/help/
	$(QUIET) echo Creating menu entry and desktop icon
	$(QUIET) install -m 644 -o `cat .user.user` -g `cat .user.group` \
		images/GNU\ Gaming\ Zone `cat .user.homedir`/Desktop/
	$(QUIET) install -m 644 images/kggz.desktop $(KDEDIR)/share/applnk/Games/
	$(QUIET) echo Done! Have fun playing GGZ!

clean:
	$(QUIET) echo Clean up on disk.
	$(QUIET) rm -rf *.o
	$(QUIET) rm -rf *.moc.cpp
	$(QUIET) rm -f kggz

distclean:
	$(QUIET) echo Remove everything.
	$(QUIET) rm -rf ../tmp.12345
	$(QUIET) rm -rf ../kggz-$(VERSION).tar.gz
	$(QUIET) rm -rf $(DESTDIR)/bin/kggz
	$(QUIET) rm -rf $(DESTDIR)/share/kggz/
	$(QUIET) rm -rf `cat .user.homedir`/Desktop/GNU\ Gaming\ Zone
	$(QUIET) rm -rf $(KDEDIR)/share/applnk/Games/kggz.desktop
	$(QUIET) rm -rf `cat .user.homedir`/.kde/share/icons/kggz.png
	$(QUIET) rm -rf `cat .user.homedir`/.kde/share/icons/kggz_small.png
	$(QUIET) rm -rf .user.*


# Only for maintainers: building packages
clean_tmp:
	# TODO: select files for release separately
	$(QUIET) echo Clean up temporarily
	$(QUIET) mkdir ../tmp.12345
	$(QUIET) touch tmp.12345.o
	$(QUIET) touch tmp.12345.moc.cpp
	$(QUIET) touch tmp.12345.kggz
	$(QUIET) mv -f *.o ../tmp.12345
	$(QUIET) mv -f *.moc.cpp ../tmp.12345
	$(QUIET) mv -f *kggz ../tmp.12345

unclean_tmp:
	$(QUIET) echo Restore temporary files
	$(QUIET) mv -f ../tmp.12345/* .
	$(QUIET) rm -rf ../tmp.12345
	$(QUIET) rm -rf tmp.12345.*

package:
	$(QUIET) echo Make a release; \
	cd ..; \
	mv kde kggz-$(VERSION); \
	tar -cf kggz-$(VERSION).tar kggz-$(VERSION); \
	mv kggz-$(VERSION) kde; \
	gzip --force --best kggz-$(VERSION).tar; \
	cd kde; \
	echo Release $(VERSION) built successfully!

upload:
	$(QUIET) echo Putting KGGZ on mindx.sourceforge.net
	$(QUIET) scp ../kggz-$(VERSION).tar.gz dr_maux@mindx.sourceforge.net:/home/groups/ftp/pub/mindx/
	$(QUIET) rm ../kggz-$(VERSION).tar.gz
	$(QUIET) echo Ready!
