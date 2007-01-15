# $Id: reversi.dsc 8921 2007-01-15 15:13:14Z josef $

[GameInfo]
Author = Ismael Orenstein
Description = Simple GGZ game module for playing Reversi
Homepage = http://www.ggzgamingzone.org/gameservers/reversi/
Name = Reversi
Version = 0.0.8

[LaunchInfo]
ExecutablePath = ggzd.reversi
# Set ExecutableArgs in the room file

[Protocol]
Engine = Reversi
Version = 4

[TableOptions]
AllowLeave = 1
BotsAllowed = 1
PlayersAllowed = 2

[Statistics]
Records = 1
Ratings = 1
