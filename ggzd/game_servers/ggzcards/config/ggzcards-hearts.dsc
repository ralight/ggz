# $Id: ggzcards-hearts.dsc 5066 2002-10-27 12:53:20Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Hearts
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Hearts
Version = 0.0.6

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=hearts --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 3-pre1

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 3..6
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
