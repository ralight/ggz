# $Id: ggzcards-lapocha.dsc 2352 2001-09-04 03:20:07Z rgade $
[GameInfo]
Author = Rich Gade
Description = GGZ Cards game module for La Pocha
Homepage = http://ggz.sourceforge.net
Name = GGZCards-LaPocha
Version = 0.0.4

[LaunchInfo]
ExecutablePath = ggzd.ggzcards
ArgList = --game=lapocha --option=open_hands:0
#GameDisabled = 1

[Protocol]
Engine = GGZCards
Version = 1

[TableOptions]
AllowLeave = 1
BotsAllowed = 0 1 2 3
PlayersAllowed = 4
