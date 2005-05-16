# $Id: ggzcards.dsc 7190 2005-05-16 21:10:29Z josef $
[GameInfo]
Author = Jason Short, Rich Gade
Description = GGZ Cards game module
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --option=rated_game:0

[Protocol]
Engine = GGZCards
Version = 3

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 2..6
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
