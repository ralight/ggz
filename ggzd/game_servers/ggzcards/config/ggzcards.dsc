# $Id: ggzcards.dsc 4509 2002-09-11 04:10:37Z jdorje $
[GameInfo]
Author = Jason Short, Rich Gade
Description = GGZ Cards game module
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --option=rated_game:0

[Protocol]
Engine = GGZCards
Version = 2-pre4

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 2..6
KillWhenEmpty = 0
AllowSpectators = 1
