# $Id: ggzcards-whist.dsc 7190 2005-05-16 21:10:29Z josef $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Whist
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards-Whist
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=whist --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 3

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
