# RPM Spec file for GGZ Gaming Zone server

Summary: Server software for the GGZ Gaming Zone
Name: ggz-server
Version: 0.0.5pre
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz-server-0.0.5pre.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: GGZ Dev Team <ggz-dev@lists.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The GGZ Gaming Zone server allows other computers to connect to yours via
the Internet and play network games.  Currently, the following game servers
are packaged with GGZ:
  - Spades		- Connect the Dots
  - Tic-Tac-Toe		- La Pocha
  - Chinese Checkers	- Chess
  - Combat		- Hastings
  - Krosswater		- Reversi
  - GGZ Cards		- Escape

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
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README README.GGZ TODO doc

%dir /etc/ggzd
%config /etc/ggzd/ggzd.conf
%config /etc/ggzd/ggzd.motd
%dir /etc/ggzd/games
%config /etc/ggzd/games/ccheckers.dsc
%config /etc/ggzd/games/chess.dsc
%config /etc/ggzd/games/combat.dsc
%config /etc/ggzd/games/dots.dsc
%config /etc/ggzd/games/escape.dsc
%config /etc/ggzd/games/ggzcards.dsc
%config /etc/ggzd/games/ggzcards-bridge.dsc
%config /etc/ggzd/games/ggzcards-hearts.dsc
%config /etc/ggzd/games/ggzcards-lapocha.dsc
%config /etc/ggzd/games/ggzcards-spades.dsc
%config /etc/ggzd/games/ggzcards-suaro.dsc
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
%config /etc/ggzd/rooms/entry.room
%config /etc/ggzd/rooms/escape.room
%config /etc/ggzd/rooms/ggzcards.room
%config /etc/ggzd/rooms/ggzcards-bridge.room
%config /etc/ggzd/rooms/ggzcards-hearts.room
%config /etc/ggzd/rooms/ggzcards-lapocha.room
%config /etc/ggzd/rooms/ggzcards-spades.room
%config /etc/ggzd/rooms/ggzcards-suaro.room
%config /etc/ggzd/rooms/hastings.room
%config /etc/ggzd/rooms/krosswater.room
%config /etc/ggzd/rooms/lapocha.room
%config /etc/ggzd/rooms/reversi.room
%config /etc/ggzd/rooms/spades.room
%config /etc/ggzd/rooms/tictactoe.room

/usr/bin/ggzd
/usr/lib/ggzd

# the versioning information here needs to be fixed before 0.0.5
/usr/lib/libggzdmod.a
/usr/lib/libggzdmod.la
/usr/lib/libggzdmod.so
/usr/lib/libggzdmod.so.0
/usr/lib/libggzdmod.so.0.0.0

# the libzoneserver is a different (and out-of-date) variant of libggzdmod.
/usr/lib/libzoneserver.a
/usr/lib/libzoneserver.la
/usr/lib/libzoneserver.so
/usr/lib/libzoneserver.so.0
/usr/lib/libzoneserver.so.0.0.1

/usr/man/man6/ggzd.6.gz

%changelog
* Wed Sep 12 2001 Jason Short <jdorje@users.sourceforge.net>
- Updated files and miscellany for 0.0.5pre

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
