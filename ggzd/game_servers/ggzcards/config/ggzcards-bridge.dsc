# $Id: ggzcards-bridge.dsc 2296 2001-08-28 04:33:14Z rgade $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Bridge
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Bridge
Version = 0.0.4

[LaunchOptions]
ExecutablePath = ggzd.ggzcards --game=bridge --option=open_hands:0
# GameDisabled = 1

[Protocol]
Engine = GGZCards
Version = 1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
