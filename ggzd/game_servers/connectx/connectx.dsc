# $Id: connectx.dsc 8921 2007-01-15 15:13:14Z josef $

[GameInfo]
Author = Roger Light
Description = Place X tiles in a row.
Homepage = http://www.ggzgamingzone.org/gameservers/connectx/
Name = ConnectX
Version = 0.0.2

[LaunchInfo]
ExecutablePath = ggzd.connectx
# Set ExecutableArgs in the room file

[Protocol]
Engine = ConnectX
Version = 2

[TableOptions]
AllowLeave = 1
BotsAllowed = 1
PlayersAllowed = 2

[NamedBots]
ConnectXInternal=connectx
Velena=velena
