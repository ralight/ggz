# $Id: ccheckers.dsc 8880 2007-01-09 17:22:14Z josef $

[GameInfo]
Author = Rich Gade
Description = GGZ game module for playing Chinese Checkers
Homepage = http://www.ggzgamingzone.org/gameservers/chinese-checkers/
Name = ccheckers
Version = 0.0.3

[LaunchInfo]
ExecutablePath = ggzd.ccheckers
# Set ExecutableArgs in the room file

[Protocol]
Engine = CCheckers
Version = 3

[TableOptions]
AllowLeave = 1
BotsAllowed = 1..5
PlayersAllowed = 2..4 6

[Statistics]
Records = 1
Ratings = 1
