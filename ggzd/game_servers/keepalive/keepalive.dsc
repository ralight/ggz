# $Id: keepalive.dsc 8254 2006-06-22 09:30:47Z oojah $

[GameInfo]
Author = Josef Spillner
Description = Experimental GGZ game: permanent games and server-side storage
Homepage = http://www.ggzgamingzone.org/games/keepalive/
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

