# $Id: ggzcards-suaro.dsc 8254 2006-06-22 09:30:47Z oojah $

[GameInfo]
Author = Jason Short
Description = GGZ Cards game module for Suaro
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards-Suaro
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=suaro
# Set ExecutableArgs in the room file

[Protocol]
Engine = GGZCards
Version = 3

[TableOptions]
AllowLeave = 1
BotsAllowed = 1
PlayersAllowed = 2
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
