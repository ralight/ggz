# $Id: ggzcards-hearts.dsc 4757 2002-09-29 11:53:43Z dr_maux $
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
Version = 2

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 3..6
KillWhenEmpty = 0
AllowSpectators = 1
