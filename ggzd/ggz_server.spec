# RPM Spec file for Gnu Gaming Zone server

Summary: Server software for the Gnu Gaming Zone
Name: ggz_server
Version: 0.0.2
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://download.sourceforge.net/GGZ/ggz_server-0.0.2.tar.gz
URL: http://ggz.sourceforge.net/
Vendor: The GGZ Development Team
Packager: Rich Gade <rgade@users.sourceforge.net>
BuildRoot: /var/tmp/%{name}-buildroot

%description
The Gnu Gaming Zone server allows other computers to connect to yours via
the Internet and play network games.  Currently, the following game servers
are included with GGZ:
  - Spades
  - Tic Tac Toe

%prep
%setup

%build
./configure --prefix=/usr --sysconfdir=/etc
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README TODO doc

%dir /etc/ggzd
%config /etc/ggzd/ggzd.conf
%config /etc/ggzd/ggzd.motd
%dir /etc/ggzd/games
%config /etc/ggzd/games/spades.dsc
%config /etc/ggzd/games/tictactoe.dsc
%dir /etc/ggzd/rooms
%config /etc/ggzd/rooms/spades.room
%config /etc/ggzd/rooms/tictactoe.room

/usr/bin/ggzd
/usr/include/easysock.h
/usr/lib/ggzd
/usr/lib/libeasysock.a
/usr/lib/libeasysock.la
/usr/lib/libeasysock.so.0.0.0

%changelog
* Thu Apr 20 2000 Rich Gade <rgade@users.sourceforge.net>
- Rewrote to use a BuildRoot in /var/tmp
- Moved sysconfdir to /etc

* Sat Apr 15 2000 Rich Gade <rgade@users.sourceforge.net>
- First stab at a .spec file for the Gnu Gaming Zone server
