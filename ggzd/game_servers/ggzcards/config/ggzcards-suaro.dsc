# $Id: ggzcards-suaro.dsc 2414 2001-09-09 02:47:49Z jdorje $
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
Version = devel-2

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1
PlayersAllowed = 2
