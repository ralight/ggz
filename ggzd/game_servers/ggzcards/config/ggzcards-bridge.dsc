# $Id: ggzcards-bridge.dsc 3997 2002-04-16 19:03:58Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Bridge
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Bridge
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=bridge --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = 2-pre1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
