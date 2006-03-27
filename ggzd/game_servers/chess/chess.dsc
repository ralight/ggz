# $Id: chess.dsc 7977 2006-03-27 08:31:51Z josef $

[GameInfo]
Author = Ismael Orenstein
Description = GGZ game module for playing Chess
Homepage = http://www.ggzgamingzone.org/games/chess/
Name = Chess
Version = 0.0.10

[LaunchInfo]
ExecutablePath = ggzd.chess

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
