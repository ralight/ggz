# $Id: ggzcards-lapocha.dsc 5066 2002-10-27 12:53:20Z jdorje $
[GameInfo]
Author = Rich Gade
Description = GGZ Cards game module for La Pocha
Homepage = http://ggz.sourceforge.net/games/ggzcards
Name = GGZCards-LaPocha
Version = 0.0.6

[LaunchInfo]
ExecutablePath = ggzd.ggzcards --game=lapocha --option=open_hands:0 --option=rated_game:1

[Protocol]
Engine = GGZCards
Version = 3-pre1

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..3
PlayersAllowed = 4
KillWhenEmpty = 0
AllowSpectators = 1

[Statistics]
Records = 1
