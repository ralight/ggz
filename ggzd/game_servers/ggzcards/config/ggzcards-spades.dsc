# $Id: ggzcards-spades.dsc 2296 2001-08-28 04:33:14Z rgade $
[GameInfo]
Author = Jason Short, Brent Hendricks
Description = GGZ game module for trick-taking card games
Homepage = http://ggz.sourceforge.net
Name = GGZCards-Spades
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=spades --option=open_hands:0
#GameDisabled = 1

[Protocol]
Engine = GGZCards
Version = 1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
