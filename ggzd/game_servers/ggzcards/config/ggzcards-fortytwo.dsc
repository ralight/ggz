# $Id: ggzcards-fortytwo.dsc 5544 2003-05-10 23:15:02Z dr_maux $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Forty-Two
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-FortyTwo
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=fortytwo --option=open_hands:0 --option=rated_game:1

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
