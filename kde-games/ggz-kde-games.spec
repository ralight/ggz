# RPM Spec file for GGZ KDE Games (Full)

Summary: GGZ Games for KDE
Name: ggz-kde-games
Version: 0.0.4
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-kde-games-0.0.4.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The complete set of Gnu Gaming Zone games for the KDE user interface.
Includes all of the following:
    KReversi		Tic-Tac-Tux		Connect the Dots
    Krosswater

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

# Get a copy of all of our .dsc files
mkdir -p $RPM_BUILD_ROOT/usr/share/ggz/ggz-config
GAMES="KReversi kdots krosswater ktictactux"
for i in $GAMES; do
  cp $i/module.dsc $RPM_BUILD_ROOT/usr/share/ggz/ggz-config/kde-$i.dsc
done


%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
PATH="$PATH:/sbin" ldconfig
# Run ggz-config vs. all installed games
GAMES="KReversi kdots krosswater ktictactux"
for i in $GAMES; do
  ggz-config --install --fromfile=/usr/share/ggz/ggz-config/kde-$i.dsc --force
done


%preun
# Run ggz-config to uninstall all the games
GAMES="KReversi kdots krosswater ktictactux"
for i in $GAMES; do
  ggz-config --remove --fromfile=/usr/share/ggz/ggz-config/kde-$i.dsc
done

%postun
PATH="$PATH:/sbin" ldconfig


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README

/usr/lib/ggz/ggz.kreversi
/usr/lib/ggz/kdots_client
/usr/lib/ggz/krosswater_client
/usr/lib/ggz/ktictactux_client

/usr/lib/libzoneclient.la
/usr/lib/libzoneclient.so
/usr/lib/libzoneclient.so.0
/usr/lib/libzoneclient.so.0.0.1

/usr/share/apps/ggz.kreversi

/usr/share/ggz/ggz-config/kde-KReversi.dsc
/usr/share/ggz/ggz-config/kde-kdots.dsc
/usr/share/ggz/ggz-config/kde-krosswater.dsc
/usr/share/ggz/ggz-config/kde-ktictactux.dsc

/usr/share/ggz/kdots
/usr/share/ggz/kreversi
/usr/share/ggz/krosswater
/usr/share/ggz/ktictactux


%changelog
* Sun Jun 17 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
