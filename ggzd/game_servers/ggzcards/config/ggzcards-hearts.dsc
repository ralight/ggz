# $Id: ggzcards-hearts.dsc 3578 2002-03-16 16:22:43Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Hearts
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Hearts
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=hearts --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-10

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3 4 5
PlayersAllowed = 3 4 5 6
KillWhenEmpty = 0
