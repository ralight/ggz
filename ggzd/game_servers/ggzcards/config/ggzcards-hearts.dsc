# $Id: ggzcards-hearts.dsc 3595 2002-03-17 00:14:56Z jdorje $
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
Version = devel-11

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3 4 5
PlayersAllowed = 3 4 5 6
KillWhenEmpty = 0
