# $Id: ggzcards-hearts.dsc 2414 2001-09-09 02:47:49Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Hearts
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Hearts
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards
ArgList = --game=hearts --option=open_hands:0
# GameDisabled = 1

[Protocol]
Engine = GGZCards
Version = devel-2

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3 4 5
PlayersAllowed = 3 4 5 6
