# $Id: ggzcards-bridge.dsc 3912 2002-04-12 18:35:15Z jdorje $
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
Version = 1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
