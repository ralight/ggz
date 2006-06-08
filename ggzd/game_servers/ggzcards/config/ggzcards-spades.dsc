# $Id: ggzcards-spades.dsc 8133 2006-06-08 03:05:32Z jdorje $
[GameInfo]
Author = Jason Short, Brent Hendricks
Description = GGZ game module for trick-taking card games
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards-Spades
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=spades
#ExecutablePath = ggzd.ggzcards --game=spades --option=open_hands:0 --option=rated_game:1 --option=nil_value:2 --option=target_score:2 --option=minimum_bid:3 --option=double_nil:0

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
