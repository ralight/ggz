# $Id: ggzcards-spades.dsc 4238 2002-06-09 06:53:46Z jdorje $
[GameInfo]
Author = Jason Short, Brent Hendricks
Description = GGZ game module for trick-taking card games
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-Spades
Version = 0.0.6pre

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=spades --option=open_hands:0 --option=rated_game:1 --option=nil_value:2 --option=target_score:2 --option=minimum_bid:3 --option=double_nil:0

[Protocol]
Engine = GGZCards
Version = 2-pre4

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
KillWhenEmpty = 0
