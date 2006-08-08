# $Id: connectx.dsc 8498 2006-08-08 09:02:27Z josef $

[GameInfo]
Author = Roger Light
Description = Place X tiles in a row.
Homepage = http://wggz.sf.net
Name = ConnectX
Version = 0.0.1

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
