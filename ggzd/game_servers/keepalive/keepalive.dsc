# $Id: keepalive.dsc 4548 2002-09-13 16:19:50Z jdorje $

[GameInfo]
Author = Josef Spillner
Description = Experimental GGZ game: permanent games and server-side storage
Homepage = http://ggz.sourceforge.net/games/keepalive/
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
PlayersAllowed = 1 2 3 4 5 6 7 8
AllowSpectators = 1
KillWhenEmpty = 0

