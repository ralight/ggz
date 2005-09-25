# $Id: tictactoe.dsc 7561 2005-09-25 12:41:12Z josef $

[GameInfo]
Author = Brent Hendricks
Description = Simple GGZ game module for playing Tic-Tac-Toe
Homepage = http://www.ggzgamingzone.org/games/tictactoe/
Name = TicTacToe
Version = 0.0.8

[LaunchInfo]
ExecutablePath = ggzd.tictactoe

[Protocol]
Engine = TicTacToe
Version = 4

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

