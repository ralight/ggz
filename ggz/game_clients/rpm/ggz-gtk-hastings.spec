# RPM Spec file for GGZ Gtk+ Games (Individual)

Summary: GGZ Hastings for Gtk 1.2
Name: ggz-gtk-hastings
Version: 0.0.4
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-gtk-hastings-0.0.4.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
A Hastings client for the Gnu Gaming Zone Gtk user interface.

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc
make


%install
# Create a ggz.modules file so we can make install easily
mkdir -p $RPM_BUILD_ROOT/etc
touch $RPM_BUILD_ROOT/etc/ggz.modules
make DESTDIR=$RPM_BUILD_ROOT install
rm $RPM_BUILD_ROOT/etc/ggz.modules
rmdir $RPM_BUILD_ROOT/etc

# Get a copy of all of our .dsc file
mkdir -p $RPM_BUILD_ROOT/usr/share/ggz/ggz-config
cp hastings/module.dsc $RPM_BUILD_ROOT/usr/share/ggz/ggz-config/gtk-hastings.dsc


%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
# Run ggz-config vs. the now installed game
ggz-config --install --fromfile=/usr/share/ggz/ggz-config/gtk-hastings.dsc --force


%preun
# Run ggz-config to uninstall the game
ggz-config --remove --fromfile=/usr/share/ggz/ggz-config/gtk-hastings.dsc


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README

/usr/lib/ggz/ggz.hastings
/usr/share/ggz/ggz-config/gtk-hastings.dsc


%changelog
* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
