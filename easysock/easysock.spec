# RPM Spec file for Easysock library

Summary: easysock socket library
Name: easysock
Version: 0.2.0
Release: 1
Copyright: GPL
Group: Development/Libraries
Source: http://download.sourceforge.net/GGZ/easysock-0.2.0.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
easysock is a library which simplifies TCP/IP socket communications
for both clients and servers.

%prep
%setup

%build
./configure --prefix=/usr
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
rm -f /usr/lib/libeasysock.so.0

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO

/usr/lib/libeasysock.a
/usr/lib/libeasysock.la
/usr/lib/libeasysock.so.0.2.0
/usr/include/easysock.h

%changelog
* Tue Mar 27 2001 Rich Gade <rgade@users.sourceforge.net>
- Modified for version bump to 0.2.0
- Modified for name change (libeasysock -> easysock)
- Modified to remove the stale libeasysock.so.0 link on uninstall

* Sun Mar 25 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
