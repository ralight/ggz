# $Id: ggzcards-spades.dsc 3421 2002-02-19 10:59:53Z jdorje $
[GameInfo]
Author = Jason Short, Brent Hendricks
Description = GGZ game module for trick-taking card games
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Spades
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=spades --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-6

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
