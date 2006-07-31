# $Id: ggzcards-lapocha.dsc 8427 2006-07-31 22:50:50Z jdorje $
[GameInfo]
Author = Rich Gade
Description = GGZ Cards game module for La Pocha
Homepage = http://www.ggzgamingzone.org/games/ggzcards/
Name = GGZCards-LaPocha
Version = 0.0.7

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=lapocha
# Set ExecutableArgs in the room file

[Protocol]
Engine = GGZCards
Version = 5

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
Ratings = 1
