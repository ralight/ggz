# $Id: ggzcards-bridge.dsc 3412 2002-02-18 09:02:33Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Bridge
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Bridge
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=bridge --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-5

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
