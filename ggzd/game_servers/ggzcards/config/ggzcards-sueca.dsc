# $Id: ggzcards-sueca.dsc 8259 2006-06-23 06:53:15Z jdorje $
[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Sueca
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards-Sueca
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=sueca
# Set ExecutableArgs in the room file

[Protocol]
Engine = GGZCards
Version = 4

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
