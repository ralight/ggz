# $Id: ggzcards-sueca.dsc 4509 2002-09-11 04:10:37Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Sueca
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Sueca
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=sueca --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 2-pre4

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1
