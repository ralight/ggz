# $Id: ggzcards-spades.dsc 3595 2002-03-17 00:14:56Z jdorje $
[GameInfo]
Author = Jason Short, Brent Hendricks
Description = GGZ game module for trick-taking card games
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Spades
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=spades --option=open_hands:0

[Protocol]
Engine = GGZCards
Version = devel-11

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
