# $Id: ggzcards.dsc 5544 2003-05-10 23:15:02Z dr_maux $
[GameInfo]
Author = Jason Short, Rich Gade
Description = GGZ Cards game module
Homepage = http://ggz.sourceforge.net/games/ggzcards
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
