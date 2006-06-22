# $Id: spades.dsc 8254 2006-06-22 09:30:47Z oojah $

[GameInfo]
Author = Brent Hendricks
Description = NetSpades is a multiuser networked spades game
Homepage = http://www.ece.rice.edu/~brentmh/spades
Name = NetSpades
Version = 0.0.5

[LaunchInfo]
ExecutablePath = ggzd.spades
# Set ExecutableArgs in the room file

[Protocol]
Engine = NetSpades
Version = 2

[TableOptions]
AllowLeave = 0
BotsAllowed = 1..3
PlayersAllowed = 4
