# $Id: ggzcards-whist.dsc 4153 2002-05-05 00:42:40Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Whist
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Whist
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=whist --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 2-pre4

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
