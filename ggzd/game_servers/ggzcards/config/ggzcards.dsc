# $Id: ggzcards.dsc 4988 2002-10-22 08:23:04Z jdorje $
[GameInfo]
Author = Jason Short, Rich Gade
Description = GGZ Cards game module
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards
Version = 0.0.6

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --option=rated_game:0

[Protocol]
Engine = GGZCards
Version = 3-pre1

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 2..6
KillWhenEmpty = 0
AllowSpectators = 1
