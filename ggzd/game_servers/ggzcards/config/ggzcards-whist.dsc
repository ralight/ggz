# $Id: ggzcards-whist.dsc 5081 2002-10-28 04:58:39Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Whist
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Whist
Version = 0.0.6

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=whist --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 3-pre1

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
