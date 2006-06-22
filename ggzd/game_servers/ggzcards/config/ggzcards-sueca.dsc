# $Id: ggzcards-sueca.dsc 8254 2006-06-22 09:30:47Z oojah $
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
