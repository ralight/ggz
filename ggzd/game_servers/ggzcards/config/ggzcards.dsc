# $Id: ggzcards.dsc 4757 2002-09-29 11:53:43Z dr_maux $
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
Version = 2

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 2..6
KillWhenEmpty = 0
AllowSpectators = 1
