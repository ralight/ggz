#include "game.h"
#include "game.moc"

#include <iostream>
#include <kdebug.h>

// quickhack!
#define GGZ_SEAT_OPEN -1

Game::Game(void)
{
	kdDebug(12101) << "Game::Game()" << endl;

	ggz = new GGZ;
	CHECK_PTR(ggz);
	ggz->connect("chess");
	connect(ggz, SIGNAL(recvData()), SLOT(handleNetInput()));
	
	chessGame = cgc_create_game();
	cgc_join_game(chessGame, WHITE);
	cgc_join_game(chessGame, BLACK);

	chessInfo.state = CHESS_STATE_WAIT;
	chessInfo.clock_type = 0;
	chessInfo.seconds[0] = 0;
	chessInfo.seconds[1] = 0;
	chessInfo.t_seconds[0] = 0;
	chessInfo.t_seconds[1] = 0;
	chessInfo.turn = 0;
	chessInfo.check = FALSE;
	chessInfo.name[0] = "White";
	chessInfo.name[1] = "Black";

	//board = new ChessBoard;
	//CHECK_PTR(board);

	//board->show();
}

Game::~Game(void)
{
	if (ggz)
		delete ggz;
	//if (board)
		//delete board;
	ggz = NULL;
	//board = 0;
}

int swap(int value)
{
	int ret;
	ret = (value & 0xFF) << 24 +
		((value >> 8) & 0xFF) << 16 +
		((value >> 16) & 0xFF) << 8 +
		((value >> 24) & 0xFF);
	return ret;
}

void Game::handleNetInput(void)
{
	char opcode, value;
	char cval;
	QString s;
	int x, y, x2, y2;

	if ((opcode = ggz->getChar()) < 0)
		return;

	kdDebug(12101) << "Game::handleNetInput(); opcode: " << opcode << endl;

	//opcode = swap(opcode);
	//kdDebug(12101) << "Game::handleNetInput(); opcode is now: " << opcode << endl;

	switch (opcode)
	{
		case CHESS_MSG_SEAT:
			kdDebug(12101) << "Received MSG_SEAT" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;

			chessInfo.seat = ggz->getChar();
			chessInfo.version = ggz->getChar();
			kdDebug(12101) << "Got seat " << chessInfo.seat << " and version " << chessInfo.version << endl;
			emit signalMessage("Received seat.");

			if (chessInfo.version != PROTOCOL_VERSION)
				kdDebug(12101) << "Incompatible version. The game may not run as expected" << endl;

			emit signalStart(chessInfo.seat);
			break;
		case CHESS_MSG_PLAYERS:
			kdDebug(12101) << "Got an MSG_PLAYERS" << endl;

			if (GGZ_SEAT_OPEN != (chessInfo.assign[0] = ggz->getChar()))
				chessInfo.name[0] = ggz->getString();

			if (GGZ_SEAT_OPEN != (chessInfo.assign[1] = ggz->getChar()))
				chessInfo.name[1] = ggz->getString();

			kdDebug(12101) << "Got players " << chessInfo.name[0] << " and " << chessInfo.name[1] << endl;
			emit signalMessage("Received player names.");
			break;
		case CHESS_REQ_TIME:
			kdDebug(12101) << "Got an REQ_TIME" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
        			break;

			emit signalMessage("Received clock request.");
			emit signalNewGame();
			break;
		case CHESS_RSP_TIME:
      			kdDebug(12101) << "Got an RSP_TIME" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;

			value = ggz->getInt();
			chessInfo.clock_type = value >> 24;
			chessInfo.seconds[0] = value & 0xFFFFFF;
			chessInfo.seconds[1] = value & 0xFFFFFF;
			chessInfo.t_seconds[0] = value & 0xFFFFFF;
			chessInfo.t_seconds[1] = value & 0xFFFFFF;
			switch(chessInfo.clock_type)
			{
				case CHESS_CLOCK_NOCLOCK:
					kdDebug(12101) << "This game won't have a time limit." << endl;
					emit signalMessage("Clock: no limit.");
					break;
				case CHESS_CLOCK_CLIENT:
					kdDebug(12101) << "This game will use a client clock.\nThis option should only be used when playing against people you trust, as it relies much in the client program, that can be cheated.\nSo, if the time behaves very strangely (like your oponnent time never wearing out), he may be running a cheated client.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
					emit signalMessage("Clock: client-side.");
					break;
				case CHESS_CLOCK_SERVER:
					kdDebug(12101) << "This game will use a server clock.\nIt is very difficult to cheat when using this type of clock, and you should use it if you suspect your oponnent may have a cheated client or if you don't trust him.\nHowever, if either your connection or your opponent's is deeply lagged, it will have a deep effect on the time count as well.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
					emit signalMessage("Clock: server-side.");
					break;
				case CHESS_CLOCK_SERVERLAG:
					kdDebug(12101) << "This game will use a server clock with lag support.\nIn this option, we will use a server clock, but using a lag meter to compensate for any lag due to Internet connection. Although it's possible to cheat with this option, it is much more difficult then cheating with the client clock.\nBesides, the lag of either connect won't have a so deep effect on the time of the players.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
					emit signalMessage("Clock: server-side with lag.");
					break;
				default:
					kdDebug(12101) << "Clock type is " << chessInfo.clock_type << " and time is " << chessInfo.seconds[0] << " seconds." << endl;
					emit signalMessage("Clock: Error - unexpected type.");
					break;
			}
			break;
		case CHESS_MSG_START:
			kdDebug(12101) << "Got an MSG_START" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;
			chessInfo.state = CHESS_STATE_PLAYING;

			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
				chessInfo.timer->changeInterval(1000);

			kdDebug(12101) << "The game has started!" << endl;
			emit signalMessage("Game: started.");
			break;

		case CHESS_MSG_MOVE:
			kdDebug(12101) << "Got an MSG_MOVE" << endl;
			//s = ggz->getString(6);
			kdDebug(12101) << "Args: ";
			ggz->getChar();ggz->getChar();ggz->getChar();ggz->getChar(); // FIXME: read string length
			x = ggz->getChar(); kdDebug(12101) << x;
			y = ggz->getChar(); kdDebug(12101) << y;
			x2 = ggz->getChar(); kdDebug(12101) << x2;
			y2 = ggz->getChar(); kdDebug(12101) << y2;
			cval = ggz->getChar(); kdDebug(12101) << cval; // should be 0 no?
			//cval = ggz->getChar(); kdDebug(12101) << cval;
			kdDebug(12101) << endl;
			//es_read_string(chess.fd, args, 6);
			/* Should we worry about time ? */
			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
				//es_read_int(chess.fd, (gint*)args+2);
				ggz->getInt();
//			game_update(CHESS_EVENT_MOVE, args.latin1());
			if((x >= 0) && (y >= 0) && (x2 >= 0) && (y2 >= 0))
				emit signalMove(QString("Net: from %1/%2 to %3/%4").arg(QChar(x)).arg(QChar(y)).arg(QChar(x2)).arg(QChar(y2)));
			else
				emit signalMessage("Invalid move - try again!");
			break;

		case CHESS_MSG_GAMEOVER:
			/* The game is over */
			kdDebug(12101) << "Got a MSG_GAMEOVER" << endl;
			//es_read_char(chess.fd, &args[0]);
			cval = ggz->getChar();
			kdDebug(12101) << "Gameover msg: " << cval << endl;
//			game_update(CHESS_EVENT_GAMEOVER, args);
			break;

		case CHESS_REQ_DRAW:
			/* Do you want to draw the game ? */
			kdDebug(12101) << "Got a REQ_DRAW" << endl;
//			game_update(CHESS_EVENT_DRAW, NULL);
			break;

		case CHESS_RSP_UPDATE:
			/* We want to update the seconds */
			kdDebug(12101) << "Got an RSP_UPDATE" << endl;
			//es_read_int(fd, (int*)args);
			value = ggz->getInt();
			//es_read_int(fd, (int*)args+1);
			value = ggz->getInt();
//			game_update(CHESS_EVENT_UPDATE_TIME, args);
			break;

		default:
			kdDebug(12101) << "Unknown opcode: " << opcode << endl;
			break;
	}
}

void Game::setTime(int time)
{
	kdDebug(12101) << "Game::setTime(); time = " << time << endl;
	chessInfo.clock_type = (time >> 24) & 0xFF;
	ggz->putChar(CHESS_RSP_TIME);
	ggz->putInt(time);
}

void Game::slotMove(int x, int y, int x2, int y2)
{
	kdDebug(12101) << "Game::slotMove(); got move: " << x << ", " << y << " => " << x2 << ", " << y2 << endl;
	ggz->putChar(CHESS_REQ_MOVE);
	ggz->putChar(0);ggz->putChar(0);ggz->putChar(0);ggz->putChar(6);// FIXME: string handling
	ggz->putChar(x + 'A');
	ggz->putChar(y + '1');
	ggz->putChar(x2 + 'A');
	ggz->putChar(y2 + '1');
	ggz->putChar(0);
	ggz->putChar(0);
	emit signalMove(QString("%1: from %2/%3 to %4/%5").arg((chessInfo.seat ? "Black" : "White")).arg(
		QChar(x + 'A')).arg(QChar(y + '1')).arg(QChar(x2 + 'A')).arg(QChar(y2 + '1')));
}

