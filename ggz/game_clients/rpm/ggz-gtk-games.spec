# RPM Spec file for GGZ Gtk+ Games (Full)

Summary: GGZ Games for Gtk 1.2
Name: ggz-gtk-games
Version: 0.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-gtk-games-0.0.5.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The complete set of Gnu Gaming Zone games for the Gtk 1.2 user interface.
Includes all of the following:
    Chess		Chinese Checkers	Combat
    Connect the Dots	GGZCards		Hastings
    La Pocha		Reversi			Spades
    Tic-Tac-Toe

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
GAMES="chess chinese-checkers combat dots ggzcards hastings
       lapocha reversi spades tictactoe"
for i in $GAMES; do
  cp $i/module.dsc $RPM_BUILD_ROOT/usr/share/ggz/ggz-config/gtk-$i.dsc
done


%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
# Run ggz-config vs. all installed games
GAMES="chess chinese-checkers combat dots  ggzcards hastings
       lapocha reversi spades tictactoe"
for i in $GAMES; do
  ggz-config --install --modfile=/usr/share/ggz/ggz-config/gtk-$i.dsc --force
done


%preun
# Run ggz-config to uninstall all the games
GAMES="chess chinese-checkers combat dots  ggzcards hastings
       lapocha reversi spades tictactoe"
for i in $GAMES; do
  ggz-config --remove --modfile=/usr/share/ggz/ggz-config/gtk-$i.dsc
done


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README README.GGZ QuickStart.GGZ TODO

/usr/lib/ggz

/usr/share/ggz/ccheckers
/usr/share/ggz/combat
/usr/share/ggz/ggz-config


%changelog
* Fri Apr 12 2002 Rich Gade <rgade@users.sourceforge.net>
- Updated for 0.0.5

* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Added the pre-uninstall script
- Made included files more explicit where we don't absolutely own the dir
- Added gtk- prefix to dsc files

* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
