# $Id: keepalive.dsc 8880 2007-01-09 17:22:14Z josef $

[GameInfo]
Author = Josef Spillner
Description = Experimental GGZ game: permanent games and server-side storage
Homepage = http://www.ggzgamingzone.org/gameservers/keepalive/
Name = Keepalive
Version = 0.1

[LaunchInfo]
ExecutablePath = keepalivesrv --ggz
# Set ExecutableArgs in the room file

[Protocol]
Engine = Keepalive
Version = 1

[TableOptions]
AllowLeave = 1
#BotsAllowed = 
PlayersAllowed = 1..100
AllowSpectators = 1
KillWhenEmpty = 0

