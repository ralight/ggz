# RPM Spec file for GGZ client libraries

Summary: GGZ Client for Gtk 1.2
Name: ggz-gtk-client
Version: 0.0.4
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-gtk-client-0.0.4.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The official Gnu Gaming Zone client for the Gtk 1.2 user interface.

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
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO

/usr/bin/ggz-gtk

%changelog
* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Updated for new distro name ggz-gtk-client

* Tue Mar 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
