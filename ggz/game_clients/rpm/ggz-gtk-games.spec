# RPM Spec file for GGZ Gtk+ Games (Full)

Summary: GGZ Games for Gtk 1.2
Name: ggz-gtk-games
Version: 0.0.4pre
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-gtk-games-0.0.4pre.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The complete set of Gnu Gaming Zone games for the Gtk 1.2 user interface.
Includes all of the following:
    Chess		Chinese Checkers	Combat
    Connect the Dots	Hastings		La Pocha
    Reversi		Spades			Tic-Tac-Toe

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
GAMES="chess chinese-checkers combat dots hastings lapocha reversi
       spades tictactoe"
for i in $GAMES; do
  cp $i/module.dsc $RPM_BUILD_ROOT/usr/share/ggz/ggz-config/gtk-$i.dsc
done


%clean
# WARNING - Does not test for a stupid BuildRoot
rm -rf $RPM_BUILD_ROOT


%post
# Run ggz-config vs. all installed games
GAMES="chess chinese-checkers combat dots hastings lapocha reversi
       spades tictactoe"
for i in $GAMES; do
  ggz-config --install --fromfile=/usr/share/ggz/ggz-config/gtk-$i.dsc --force
done


%preun
# Run ggz-config to uninstall all the games
GAMES="chess chinese-checkers combat dots hastings lapocha reversi
       spades tictactoe"
for i in $GAMES; do
  ggz-config --remove --fromfile=/usr/share/ggz/ggz-config/gtk-$i.dsc
done


%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README

/usr/lib/ggz/ggz.ccheckers
/usr/lib/ggz/ggz.chess
/usr/lib/ggz/ggz.combat
/usr/lib/ggz/ggz.dots
/usr/lib/ggz/ggz.hastings
/usr/lib/ggz/ggz.lapocha
/usr/lib/ggz/ggz.reversi
/usr/lib/ggz/ggz.spades
/usr/lib/ggz/ggz.ttt

/usr/share/ggz/ggz-config/gtk-chess.dsc
/usr/share/ggz/ggz-config/gtk-chinese-checkers.dsc
/usr/share/ggz/ggz-config/gtk-combat.dsc
/usr/share/ggz/ggz-config/gtk-dots.dsc
/usr/share/ggz/ggz-config/gtk-hastings.dsc
/usr/share/ggz/ggz-config/gtk-lapocha.dsc
/usr/share/ggz/ggz-config/gtk-reversi.dsc
/usr/share/ggz/ggz-config/gtk-spades.dsc
/usr/share/ggz/ggz-config/gtk-tictactoe.dsc

/usr/share/ggz/ccheckers
/usr/share/ggz/combat


%changelog
* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Added the pre-uninstall script
- Made included files more explicit where we don't absolutely own the dir
- Added gtk- prefix to dsc files

* Sun Apr 22 2001 Rich Gade <rgade@users.sourceforge.net>
- Initial specification file
