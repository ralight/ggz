# RPM Spec file for GGZ client libraries

Summary: GGZ Client for KDE
Name: ggz-kde-client
Version: 0.0.4pre6
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-kde-client-0.0.4pre6.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The official Gnu Gaming Zone client for the KDE user interface.

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc
make

%install
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
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README

/usr/bin/kggz

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
/usr/lib/libggzcorepp.so.0
/usr/lib/libggzcorepp.so.0.0.1

/usr/share/applnk/Games/kggz.desktop

/usr/share/apps/kggz

/usr/share/icons/locolor/16x16/apps/kggz.png
/usr/share/icons/locolor/32x32/apps/kggz.png

%changelog
* Fri Apr 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
