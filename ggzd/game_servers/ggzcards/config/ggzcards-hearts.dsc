# $Id: ggzcards-hearts.dsc 4108 2002-04-29 05:29:32Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Hearts
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Hearts
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=hearts --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = 2-pre4

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3 4 5
PlayersAllowed = 3 4 5 6
KillWhenEmpty = 0
