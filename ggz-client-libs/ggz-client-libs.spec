# RPM Spec file for GGZ client libraries

Summary: GGZ Client Libraries
Name: ggz-client-libs
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-client-libs-0.0.5.tar.gz
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
PATH="$PATH:/sbin" ldconfig

%postun
PATH="$PATH:/sbin" ldconfig

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README HACKING TODO QuickStart.GGZ README.GGZ

%config /etc/ggz.modules

/usr/bin/ggz-config

/usr/include/ggzcore.h
/usr/lib/libggzcore.a
/usr/lib/libggzcore.la
/usr/lib/libggzcore.so
/usr/lib/libggzcore.so.1
/usr/lib/libggzcore.so.1.0.0

/usr/include/ggzmod.h
/usr/lib/libggzmod.a
/usr/lib/libggzmod.la
/usr/lib/libggzmod.so
/usr/lib/libggzmod.so.0
/usr/lib/libggzmod.so.0.0.0

/usr/man/man3/ggzcore_h.3.gz
/usr/man/man3/ggzmod_h.3.gz
/usr/man/man5/ggz.modules.5.gz
/usr/man/man6/ggz-config.6.gz

%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.5 release

* Sun Jun 17 2001 Rich Gade <rgade@users.sourceforge.net>
- Added manual pages

* Wed Mar 28 2001 Rich Gade <rgade@users.sourceforge.net>
- Corrected spec file to properly install libggzcore as shared

* Tue Mar 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
