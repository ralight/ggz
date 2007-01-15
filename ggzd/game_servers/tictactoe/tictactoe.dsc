# $Id: tictactoe.dsc 8921 2007-01-15 15:13:14Z josef $

[GameInfo]
Author = Brent Hendricks
Description = Simple GGZ game module for playing Tic-Tac-Toe
Homepage = http://www.ggzgamingzone.org/gameservers/tictactoe/
Name = TicTacToe
Version = 0.0.10

[LaunchInfo]
ExecutablePath = ggzd.tictactoe
# Set ExecutableArgs in the room file

[Protocol]
Engine = TicTacToe
Version = 5

[TableOptions]
AllowLeave = 1
BotsAllowed = 1
PlayersAllowed = 2
AllowSpectators = 1

[Statistics]
Records = 1

[NamedBots]
Alfred=easy
Tarantula=hard

