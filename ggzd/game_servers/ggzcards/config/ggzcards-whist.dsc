# $Id: ggzcards-whist.dsc 4757 2002-09-29 11:53:43Z dr_maux $
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
Version = 2

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1
