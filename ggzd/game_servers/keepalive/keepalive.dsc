# $Id: keepalive.dsc 7433 2005-08-15 09:55:03Z josef $

[GameInfo]
Author = Josef Spillner
Description = Experimental GGZ game: permanent games and server-side storage
Homepage = http://www.ggzgamingzone.org/games/keepalive/
Name = Keepalive
Version = 0.1

[LaunchInfo]
ExecutablePath = keepalivesrv --ggz

[Protocol]
Engine = Keepalive
Version = 1

[TableOptions]
AllowLeave = 1
#BotsAllowed = 
PlayersAllowed = 1..100
AllowSpectators = 1
KillWhenEmpty = 0

