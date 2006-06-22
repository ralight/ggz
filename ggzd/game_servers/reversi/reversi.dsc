# $Id: reversi.dsc 8254 2006-06-22 09:30:47Z oojah $

[GameInfo]
Author = Ismael Orenstein
Description = Simple GGZ game module for playing Reversi
Homepage = http://www.ggzgamingzone.org/games/reversi/
Name = Reversi
Version = 0.0.7

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
