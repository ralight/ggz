# RPM Spec file for GGZ client libraries

Summary: GGZ Client for Gtk 1.2
Name: ggz-gtk-client
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-gtk-client-0.0.5.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: GGZ Dev Team <ggz-dev@lists.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The official GGZ Gaming Zone client for the Gtk 1.2 user interface.

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

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO README.GGZ QuickStart.GGZ

/usr/bin/ggz-gtk

/usr/man/man6/ggz-gtk.6.gz

%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.5

* Fri Sep 14 2001 Jason Short <jdorje@users.sourceforge.net>
- Updated for 0.0.5pre version.

* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Updated for new distro name ggz-gtk-client

* Tue Mar 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
