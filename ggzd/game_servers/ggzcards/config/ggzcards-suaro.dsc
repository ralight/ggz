# $Id: ggzcards-suaro.dsc 2352 2001-09-04 03:20:07Z rgade $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Suaro
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Suaro
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards
ArgList = --game=suaro --option=open_hands:0
#GameDisabled = 1

[Protocol]
Engine = GGZCards
Version = 1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1
PlayersAllowed = 2
