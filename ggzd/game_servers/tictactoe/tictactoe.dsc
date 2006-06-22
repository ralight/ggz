# $Id: tictactoe.dsc 8254 2006-06-22 09:30:47Z oojah $

[GameInfo]
Author = Brent Hendricks
Description = Simple GGZ game module for playing Tic-Tac-Toe
Homepage = http://www.ggzgamingzone.org/games/tictactoe/
Name = TicTacToe
Version = 0.0.9

[LaunchInfo]
ExecutablePath = ggzd.tictactoe
# Set ExecutableArgs in the room file

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

