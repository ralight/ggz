# $Id: ggzcards-suaro.dsc 4757 2002-09-29 11:53:43Z dr_maux $

[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Suaro
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Suaro
Version = 0.0.6

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=suaro --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 2

[TableOptions]
AllowLeave = 1
BotsAllowed = 1
PlayersAllowed = 2
KillWhenEmpty = 0
AllowSpectators = 1
