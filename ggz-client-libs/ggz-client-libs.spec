# RPM Spec file for GGZ client libraries

Summary: GGZ Client Libraries
Name: ggz-client-libs
Version: 0.0.4pre
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-client-libs-0.0.4pre.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The GGZ client libraries are necessary for running and/or developing a
Gnu Gaming Zone client.

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

# After installing or uninstalling, run ldconfig
%post
ldconfig

%postun
ldconfig
rm -f /usr/lib/libggzcore.so.0

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README HACKING TODO

%config /etc/ggz.modules

/usr/bin/ggz-config
/usr/lib/libggzcore.a
/usr/lib/libggzcore.la
/usr/lib/libggzcore.so.0.0.0
/usr/include/ggzcore.h

%changelog
* Tue Mar 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
