# RPM Spec file for GGZ KDE Games (Full)

Summary: GGZ Games for KDE
Name: ggz-kde-games
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-kde-games-0.0.5.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The complete set of Gnu Gaming Zone games for the KDE user interface.
Includes all of the following:
    KReversi		Tic-Tac-Tux		Connect the Dots
    Krosswater		Keepalive		Koening
    Muehle

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
GAMES="KReversi kdots keepalive koenig krosswater ktictactux muehle"
for i in $GAMES; do
  cp $i/module.dsc $RPM_BUILD_ROOT/usr/share/ggz/ggz-config/kde-$i.dsc
done


%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
PATH="$PATH:/sbin" ldconfig
# Run ggz-config vs. all installed games
GAMES="KReversi kdots keepalive koenig krosswater ktictactux muehle"
for i in $GAMES; do
  ggz-config --install --modfile=/usr/share/ggz/ggz-config/kde-$i.dsc --force
done


%preun
# Run ggz-config to uninstall all the games
GAMES="KReversi kdots keepalive koenig krosswater ktictactux muehle"
for i in $GAMES; do
  ggz-config --remove --modfile=/usr/share/ggz/ggz-config/kde-$i.dsc
done

%postun
PATH="$PATH:/sbin" ldconfig


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README README.GGZ QuickStart.GGZ TODO

/usr/bin/muehle-ai.pl

/usr/lib/ggz

/usr/lib/libzoneclient.la
/usr/lib/libzoneclient.so
/usr/lib/libzoneclient.so.0
/usr/lib/libzoneclient.so.0.0.1

/usr/man/man1/ggzwrap.1.gz
/usr/man/man6/muehle-ai.pl.6.gz

/usr/share/ggz

/usr/share/applnk/Games/Board/muehle.desktop
/usr/share/applnk/Games/ggz
/usr/share/apps/ggz.kreversi
/usr/share/apps/koenig
/usr/share/icons/hicolor/16x16/apps/koenig.png
/usr/share/icons/hicolor/16x16/apps/ktictactux.png
/usr/share/icons/hicolor/32x32/actions/warnmessage.png
/usr/share/icons/hicolor/32x32/apps/kdots.png
/usr/share/icons/hicolor/32x32/apps/krosswater.png
/usr/share/icons/hicolor/32x32/apps/ktictactux.png
/usr/share/icons/hicolor/32x32/apps/muehle.png
/usr/share/icons/hicolor/48x48/apps/ktictactux.png
/usr/share/icons/hicolor/64x64/apps/muehle.png

/usr/share/locale/de/LC_MESSAGES/ggz.kreversi.mo
/usr/share/locale/de/LC_MESSAGES/kdots.mo
/usr/share/locale/de/LC_MESSAGES/koenig.mo
/usr/share/locale/de/LC_MESSAGES/krosswater.mo
/usr/share/locale/de/LC_MESSAGES/ktictactux.mo
/usr/share/locale/de/LC_MESSAGES/muehle.mo


%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.5

* Mon Jun 18 2001 Rich Gade <rgade@users.sourceforge.net>
- Added missing dependency information (libggzcore.so.0)

* Sun Jun 17 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
