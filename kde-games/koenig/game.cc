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
	chessInfo.t_seconds[0] = 0; // FIXME: link to timer
	chessInfo.t_seconds[1] = 0;
	chessInfo.turn = 0;
	chessInfo.check = FALSE;
	chessInfo.name[0] = "White";
	chessInfo.name[1] = "Black";
}

Game::~Game(void)
{
	if (ggz)
		delete ggz;
	ggz = NULL;
}

// We don't need swap() do we?
/*int swap(int value)
{
	int ret;
	ret = (value & 0xFF) << 24 +
		((value >> 8) & 0xFF) << 16 +
		((value >> 16) & 0xFF) << 8 +
		((value >> 24) & 0xFF);
	return ret;
}*/

void Game::handleNetInput(void)
{
	char opcode, value;
	char cval;
	QString s;
	int x, y, x2, y2, time;

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
			{
				kdDebug(12101) << "Incompatible version. The game may not run as expected" << endl;
				emit signalMessage("Incompatible protocol version!");
			}

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

			time = ggz->getInt();
			chessInfo.clock_type = time >> 24;
			chessInfo.seconds[0] = time & 0xFFFFFF;
			chessInfo.seconds[1] = time & 0xFFFFFF;
			chessInfo.t_seconds[0] = time & 0xFFFFFF;
			chessInfo.t_seconds[1] = time & 0xFFFFFF;
			handleClock(chessInfo.clock_type);
			break;

		case CHESS_MSG_START:
			kdDebug(12101) << "Got an MSG_START" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;
			chessInfo.state = CHESS_STATE_PLAYING;

			//if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK) // fixme: handle timer!!
			//	chessInfo.timer->changeInterval(1000);

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
			kdDebug(12101) << endl;
			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
			{
				time = ggz->getInt();
				kdDebug(12101) << "Clock set, reading time: " << time << endl;
			}
			if((x >= 0) && (y >= 0) && (x2 >= 0) && (y2 >= 0))
			{
				emit signalMove(QString("Net: from %1/%2 to %3/%4").arg(QChar(x)).arg(QChar(y)).arg(QChar(x2)).arg(QChar(y2)));
				emit signalDoMove(x - 'A', y - '1', x2 - 'A', y2 - '1'); // FIXME: this is overhead, merge somewhere
			}
			else
				emit signalMessage("Invalid move - try again!");
			break;

		case CHESS_MSG_GAMEOVER:
			kdDebug(12101) << "Got a MSG_GAMEOVER" << endl;

			cval = ggz->getChar();
			kdDebug(12101) << "Gameover msg: " << cval << endl;
			handleGameOver(cval);
			break;

		case CHESS_REQ_DRAW:
			kdDebug(12101) << "Got a REQ_DRAW" << endl;

			emit signalDraw();
			break;

		case CHESS_RSP_UPDATE:
			kdDebug(12101) << "Got an RSP_UPDATE" << endl;

			value = ggz->getInt();
			chessInfo.seconds[0] = value;
			chessInfo.t_seconds[0] = value;
			value = ggz->getInt();
			chessInfo.seconds[1] = value;
			chessInfo.t_seconds[1] = value;
			// FIXME: reset timer
			break;

		default:
			kdDebug(12101) << "Unknown opcode: " << opcode << endl;

			emit signalMessage("Invalid network opcode!");
			break;
	}
}

void Game::setTime(int timeoption, int time)
{
	kdDebug(12101) << "Game::setTime(); timeoption = " << timeoption << ", time = " << time << endl;
	chessInfo.clock_type = timeoption;
	ggz->putChar(CHESS_RSP_TIME);
	if(timeoption) time = (timeoption << 24) + time;
	kdDebug() << "Sent time: " << time << endl;
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
	ggz->putChar(0); // promote value
	ggz->putChar(0);
	emit signalMove(QString("%1: from %2/%3 to %4/%5").arg((chessInfo.seat ? "Black" : "White")).arg(
		QChar(x + 'A')).arg(QChar(y + '1')).arg(QChar(x2 + 'A')).arg(QChar(y2 + '1')));
}

void Game::answerDraw(int draw)
{
	// FIXME: am I blind or is this actually the same as slotDraw()????
	ggz->putChar(CHESS_REQ_DRAW);
}

void Game::handleGameOver(int cval)
{
	QString s = "Game over! Reason: ";
	switch(cval)
	{
		case CHESS_GAMEOVER_DRAW_AGREEMENT:
			s += "both players agreed on a draw";
			break;
		case CHESS_GAMEOVER_DRAW_STALEMATE:
			s += "stalemate";
			break;
		case CHESS_GAMEOVER_DRAW_POSREP:
			s += "too much repetition of positions";
			break;
		case CHESS_GAMEOVER_DRAW_MATERIAL:
			s += "not enough material";
			break;
		case CHESS_GAMEOVER_DRAW_MOVECOUNT:
			s += "maximum movecount reached";
			break;
		case CHESS_GAMEOVER_DRAW_TIMEMATERIAL:
			s += "time over and no material left";
			break;
		case CHESS_GAMEOVER_WIN_1_MATE:
			s += "player 1 wins by a mate";
			break;
		case CHESS_GAMEOVER_WIN_1_RESIGN:
			s += "player 2 has resigned";
			break;
		case CHESS_GAMEOVER_WIN_1_FLAG:
			s += "player 2 has run out of time";
			break;
		case CHESS_GAMEOVER_WIN_2_MATE:
			s += "player 2 wins by a mate";
			break;
		case CHESS_GAMEOVER_WIN_2_RESIGN:
			s += "player 1 has resigned";
			break;
		case CHESS_GAMEOVER_WIN_2_FLAG:
			s += "player 1 has run out of time";
			break;
		default:
			s += "unknown reason";
	}
	emit signalMessage(s);
}

void Game::handleClock(int cval)
{
	switch(cval)
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
}

