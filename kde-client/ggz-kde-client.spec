# RPM Spec file for GGZ client libraries

Summary: GGZ Client for KDE
Name: ggz-kde-client
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-kde-client-0.0.5.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The official Gnu Gaming Zone client for the KDE user interface.

%prep
%setup

%build
# Can't do SMP builds
export MAKEFLAGS=j1
./configure --prefix=/usr --sysconfdir=/etc
make

%install
export MAKEFLAGS=j1
make DESTDIR=$RPM_BUILD_ROOT install

%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
PATH="$PATH:/sbin" ldconfig

%postun
PATH="$PATH:/sbin" ldconfig


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README DESIGN README.GGZ QuickStart.GGZ

/usr/bin/kggz
/usr/bin/ggzap
/usr/bin/ggzmodsniff
/usr/bin/shadowbridge

/usr/include/GGZCore.h
/usr/include/GGZCoreCommon.h
/usr/include/GGZCoreConf.h
/usr/include/GGZCoreConfio.h
/usr/include/GGZCoreGame.h
/usr/include/GGZCoreGametype.h
/usr/include/GGZCoreModule.h
/usr/include/GGZCorePlayer.h
/usr/include/GGZCoreRoom.h
/usr/include/GGZCoreServer.h
/usr/include/GGZCoreTable.h

/usr/lib/libggzcorepp.la
/usr/lib/libggzcorepp.so
/usr/lib/libggzcorepp.so.1
/usr/lib/libggzcorepp.so.1.0.0

/usr/lib/kde2/libkcm_ggz.la
/usr/lib/kde2/libkcm_ggz.so
/usr/lib/kde2/libkcm_ggz_default.la
/usr/lib/kde2/libkcm_ggz_default.so
/usr/lib/kde2/libkcm_ggz_games.la
/usr/lib/kde2/libkcm_ggz_games.so
/usr/lib/kde2/libkcm_ggz_ggzd.la
/usr/lib/kde2/libkcm_ggz_ggzd.so
/usr/lib/kde2/libkcm_ggz_metaserver.la
/usr/lib/kde2/libkcm_ggz_metaserver.so
/usr/lib/kde2/libkio_ggz.la
/usr/lib/kde2/libkio_ggz.so
/usr/lib/kde2/libkio_ggzmeta.la
/usr/lib/kde2/libkio_ggzmeta.so

/usr/share/applnk/Games/ggz
/usr/share/applnk/Settings/Network/kcmggz.desktop

/usr/share/apps/kggz

/usr/share/icons/hicolor/16x16/apps/ggz.png
/usr/share/icons/hicolor/16x16/apps/ggzap.png
/usr/share/icons/hicolor/16x16/apps/ggzmodsniff.png
/usr/share/icons/hicolor/16x16/apps/ggzquick.png
/usr/share/icons/hicolor/16x16/apps/kggz.png
/usr/share/icons/hicolor/32x32/apps/ggzmodsniff.png
/usr/share/icons/hicolor/32x32/apps/kggz.png

/usr/share/locale/de/LC_MESSAGES/ggzap.mo
/usr/share/locale/de/LC_MESSAGES/kcm_ggz.mo
/usr/share/locale/de/LC_MESSAGES/kggz.mo
/usr/share/locale/de/LC_MESSAGES/modsniff.mo
/usr/share/locale/de/LC_MESSAGES/shadowbridge.mo
/usr/share/locale/es/LC_MESSAGES/kggz.mo
/usr/share/locale/fr/LC_MESSAGES/kggz.mo
/usr/share/locale/pt_BR/LC_MESSAGES/kggz.mo

/usr/share/services/ggz.protocol
/usr/share/services/ggzmeta.protocol


%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.5

* Fri Apr 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
