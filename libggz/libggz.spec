# RPM Spec file for GGZ Gaming Zone common library

Summary: Common library for the GGZ Gaming Zone
Name: libggz
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/libggz-0.0.5.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: GGZ Dev Team <ggz-dev@lists.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The GGZ Gaming Zone server allows other computers to connect to yours via
the Internet and play network games.  This library provides features required
for running both clients and the server.

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var/tmp
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post
PATH="$PATH:/sbin" ldconfig

%postun
PATH="$PATH:/sbin" ldconfig

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README README.GGZ QuickStart.GGZ

/usr/include/ggz.h
/usr/include/ggz_common.h

/usr/lib/libggz.a
/usr/lib/libggz.la
/usr/lib/libggz.so
/usr/lib/libggz.so.0
/usr/lib/libggz.so.0.0.0

%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- First stab at a .spec file for the Gnu Gaming Zone common libraries
