# $Id: ggzcards-spades.dsc 3422 2002-02-19 12:04:46Z jdorje $
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
Version = devel-7

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
