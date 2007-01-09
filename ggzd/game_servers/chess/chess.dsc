# $Id: chess.dsc 8880 2007-01-09 17:22:14Z josef $

[GameInfo]
Author = Ismael Orenstein
Description = GGZ game module for playing Chess
Homepage = http://www.ggzgamingzone.org/gameservers/chess/
Name = Chess
Version = 0.0.10

[LaunchInfo]
ExecutablePath = ggzd.chess
# Set ExecutableArgs in the room file

[Protocol]
Engine = Chess
Version = 3

[TableOptions]
AllowLeave = 0
BotsAllowed = 1
PlayersAllowed = 2

[Statistics]
Records = 1
Ratings = 1

[NamedBots]
GNU Chess AI (tricky)=gnuchess
GGZ AI (easy)=ggzai
