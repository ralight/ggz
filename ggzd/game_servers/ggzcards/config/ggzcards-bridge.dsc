# $Id: ggzcards-bridge.dsc 3906 2002-04-12 06:39:42Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Bridge
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Bridge
Version = 0.0.5

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=bridge --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-13

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
