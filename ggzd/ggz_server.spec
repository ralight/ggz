# RPM Spec file for Gnu Gaming Zone server

# Due to the way the ggzd config files get built by make install, we
# have to do a true make install to /usr and can't use a BuildDir
# Because of this, builds must be done as root :(

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

%description
The Gnu Gaming Zone server allows other computers to connect to yours via
the Internet and play network games.  Currently, the following game servers
are included with GGZ:
  - Spades
  - Tic Tac Toe

%prep
%setup

%build
./configure --prefix=/usr
make

%install
make install

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog HACKING INSTALL NEWS README TODO doc

%dir /usr/etc/ggzd
%config /usr/etc/ggzd/ggzd.conf
%config /usr/etc/ggzd/ggzd.motd
%dir /usr/etc/ggzd/games
%config /usr/etc/ggzd/games/spades.dsc
%config /usr/etc/ggzd/games/tictactoe.dsc
%dir /usr/etc/ggzd/rooms
%config /usr/etc/ggzd/rooms/spades.room
%config /usr/etc/ggzd/rooms/tictactoe.room

/usr/bin/ggzd
/usr/include/easysock.h
/usr/lib/ggzd
/usr/lib/libeasysock.a
/usr/lib/libeasysock.la
/usr/lib/libeasysock.so.0.0.0

%changelog
* Sat Apr 15 2000 Rich Gade <rgade@users.sourceforge.net>
- First stab at a .spec file for the Gnu Gaming Zone server
