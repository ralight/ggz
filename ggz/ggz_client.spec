# RPM Spec file for Gnu Gaming Zone client

Summary: Client software for the Gnu Gaming Zone
Name: ggz_client
Version: 0.0.4pre
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz_client-0.0.4pre.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The Gnu Gaming Zone client allows you to connect to a GGZ server via the
Internet and play network games.  Currently the following games are included
with GGZ:
  - Spades		- Connect the Dots	- Hastings
  - Tic-Tac-Toe		- La Pocha		- Chinese Checkers
  - Reversi		- Combat		- Chess

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc
make

%install
make DESTDIR=$RPM_BUILD_ROOT install
# Blasted po subdir makefile doesn't support DESTDIR, so we'll have to
# install message catalogs manually until further notice :(
mkdir -p $RPM_BUILD_ROOT/usr/share/locale/de/LC_MESSAGES
cp po/de.gmo $RPM_BUILD_ROOT/usr/share/locale/de/LC_MESSAGES/ggz_client.mo

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README TODO

%config /etc/ggz.modules

/usr/bin/ggz
/usr/bin/ggz-config
/usr/lib/ggz
/usr/share/locale/de/LC_MESSAGES/ggz_client.mo

%changelog
%changelog
* Thu Mar 15 2001 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.4pre

* Thu Aug 31 2000 Rich Gade <rgade@users.sourceforge.net>
- Updated for release 0.0.3

* Mon Aug 21 2000 Rich Gade <rgade@users.sourceforge.net>
- Updated for version 0.0.3pre

* Mon Apr 24 2000 Brent Hendricks <bmh@users.sourceforge.net>
- Removed easysock files from list

* Thu Apr 20 2000 Rich Gade <rgade@users.sourceforge.net>
- Rewrote the server spec file for the Gnu Gaming Zone client
