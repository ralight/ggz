# $Id: ggzcards-bridge.dsc 2861 2001-12-10 20:28:44Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Bridge
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Bridge
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards
ArgList = --game=bridge --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-4

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
