# $Id: keepalive.dsc 4557 2002-09-13 18:15:12Z dr_maux $

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
PlayersAllowed = 1..100
AllowSpectators = 1
KillWhenEmpty = 0

