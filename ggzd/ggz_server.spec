# RPM Spec file for Gnu Gaming Zone server

Summary: Server software for the Gnu Gaming Zone
Name: ggz-server
Version: 0.0.4
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-server-0.0.4.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The Gnu Gaming Zone server allows other computers to connect to yours via
the Internet and play network games.  Currently, the following game servers
are included with GGZ:
  - Spades		- Connect the Dots
  - Tic-Tac-Toe		- La Pocha
  - Chinese Checkers	- Chess
  - Combat		- Hastings
  - Krosswater		- Reversi

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var/tmp
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post
PATH="$PATH:/sbin" ldconfig

%postun
PATH="$PATH:/sbin" ldconfig

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README TODO doc

%dir /etc/ggzd
%config /etc/ggzd/ggzd.conf
%config /etc/ggzd/ggzd.motd
%dir /etc/ggzd/games
%config /etc/ggzd/games/ccheckers.dsc
%config /etc/ggzd/games/chess.dsc
%config /etc/ggzd/games/combat.dsc
%config /etc/ggzd/games/dots.dsc
%config /etc/ggzd/games/hastings.dsc
%config /etc/ggzd/games/krosswater.dsc
%config /etc/ggzd/games/lapocha.dsc
%config /etc/ggzd/games/reversi.dsc
%config /etc/ggzd/games/spades.dsc
%config /etc/ggzd/games/tictactoe.dsc

%dir /etc/ggzd/rooms
%config /etc/ggzd/rooms/ccheckers.room
%config /etc/ggzd/rooms/chess.room
%config /etc/ggzd/rooms/combat.room
%config /etc/ggzd/rooms/dots.room
%config /etc/ggzd/rooms/hastings.room
%config /etc/ggzd/rooms/krosswater.room
%config /etc/ggzd/rooms/lapocha.room
%config /etc/ggzd/rooms/reversi.room
%config /etc/ggzd/rooms/spades.room
%config /etc/ggzd/rooms/tictactoe.room

/usr/bin/ggzd
/usr/lib/ggzd
/usr/lib/libzoneserver.a
/usr/lib/libzoneserver.la
/usr/lib/libzoneserver.so
/usr/lib/libzoneserver.so.0
/usr/lib/libzoneserver.so.0.0.1

/usr/man/man6/ggzd.6.gz

%changelog
* Sun Jun 17 2001 Rich Gade <rgade@users.sourceforge.net>
- Added files necessary for 0.0.4

* Thu May 31 2001 Brent Hendricks <bmh@users.sourceforge.net>
- Updated for release 0.0.4

* Thu Aug 31 2000 Rich Gade <rgade@users.sourceforge.net>
- Updated for release 0.0.3
- Moved default location for db files to /var/tmp

* Mon Aug 21 2000 Rich Gade <rgade@usrs.sourceforge.net>
- Updated for version 0.0.3pre

* Mon Apr 24 2000 Brent Hendricks <bmh@users.sourceforge.net>
- Removed easysock files from list

* Thu Apr 20 2000 Rich Gade <rgade@users.sourceforge.net>
- Rewrote to use a BuildRoot in /var/tmp
- Moved sysconfdir to /etc

* Sat Apr 15 2000 Rich Gade <rgade@users.sourceforge.net>
- First stab at a .spec file for the Gnu Gaming Zone server
