# RPM Spec file for Gnu Gaming Zone client

Summary: Client software for the Gnu Gaming Zone
Name: ggz_client
Version: 0.0.2
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz_client-0.0.2.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The Gnu Gaming Zone client allows you to connect to a GGZ server via the
Internet and play network games.  Currently the following games are included
with GGZ:
  - Spades
  - Tic Tac Toe

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README TODO

%config /etc/ggz.rc

/usr/bin/ggz
/usr/lib/ggz

%changelog
%changelog
* Mon Apr 24 2000 Brent Hendricks <bmh@users.sourceforge.net>
- Removed easysock files from list

* Thu Apr 20 2000 Rich Gade <rgade@users.sourceforge.net>
- Rewrote the server spec file for the Gnu Gaming Zone client
