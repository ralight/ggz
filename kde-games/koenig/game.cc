// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner, josef@ggzgamingzone.org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "game.h"
#include "game.moc"

#include <kdebug.h>
#include <klocale.h>

#include <ggzmod.h>
#include <ggz_common.h>
#include <iostream>

#include "ai.h"

Game::Game(bool ggzmode)
{
	kdDebug(12101) << "Game::Game()" << endl;

	if(ggzmode)
	{
		ggz = new GGZ();
		ggz->connect("chess");
		connect(ggz, SIGNAL(recvData()), SLOT(handleNetInput()));
	}
	else
	{
		ggz = NULL;
		chess_ai_init(C_WHITE, 2);
	}

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
	chessInfo.check = false;
	chessInfo.name[0] = i18n("White");
	chessInfo.name[1] = i18n("Black");
}

Game::~Game()
{
	if (ggz)
		delete ggz;
	ggz = NULL;
}

void Game::handleNetInput()
{
	char opcode, value;
	char cval;
	QString s;
	int x, y, x2, y2, time, length;

	if(!ggz) return;

	opcode = ggz->getChar();

	kdDebug(12101) << "Game::handleNetInput(); opcode: " << opcode << endl;

	switch (opcode)
	{
		case CHESS_MSG_SEAT:
			kdDebug(12101) << "Received MSG_SEAT" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;

			chessInfo.seat = ggz->getChar();
			chessInfo.version = ggz->getChar();
			kdDebug(12101) << "Got seat " << chessInfo.seat
				<< " and version " << chessInfo.version << endl;
			emit signalMessage(i18n("Received seat."));

			if (chessInfo.version != PROTOCOL_VERSION)
			{
				kdDebug(12101) << "Incompatible version. The game may not run as expected" << endl;
				emit signalMessage(i18n("Incompatible protocol version: %1 versus %2!").arg(
					chessInfo.version).arg(PROTOCOL_VERSION));
			}

			emit signalStart(chessInfo.seat);
			break;

		case CHESS_MSG_PLAYERS:
			kdDebug(12101) << "Got an MSG_PLAYERS" << endl;

			if (GGZ_SEAT_OPEN != (GGZSeatType)(chessInfo.assign[0] = ggz->getChar()))
				chessInfo.name[0] = ggz->getString();

			if (GGZ_SEAT_OPEN != (GGZSeatType)(chessInfo.assign[1] = ggz->getChar()))
				chessInfo.name[1] = ggz->getString();

			kdDebug(12101) << "Got players " << chessInfo.name[0]
				<< " and " << chessInfo.name[1] << endl;
			if (chessInfo.name[1].isEmpty())
				emit signalMessage(i18n("Received first player name: %1").arg(
					chessInfo.name[0]));
			else
				emit signalMessage(i18n("Received both player names: %1 and %2").arg(
					chessInfo.name[0]).arg(chessInfo.name[1]));
			break;

		case CHESS_REQ_TIME:
			kdDebug(12101) << "Got an REQ_TIME" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
        			break;

			emit signalMessage(i18n("Received clock request."));
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
			emit signalMessage(i18n("Game: started."));
			break;

		case CHESS_MSG_MOVE:
			kdDebug(12101) << "Got an MSG_MOVE" << endl;

			//s = ggz->getString(6);
			/*ggz->getChar();
			ggz->getChar();
			ggz->getChar();
			ggz->getChar();*/
			// FIXME: read string length
			length = ggz->getInt();
			kdDebug() << "move is " << length << " bytes long" << endl;
			if(length == 5)
			{
				x = ggz->getChar();
				y = ggz->getChar();
				x2 = ggz->getChar();
				y2 = ggz->getChar();
				cval = ggz->getChar();
			}
			else
			{
				x = -1;
				y = -1;
				x2 = -1;
				y2 = -1;
				cval = 0;
			}
			kdDebug(12101) << "Args: from " << x << ", " << y << " to "
				<< x2 << ", " << y2 << " - " << cval << endl; // cval should be 0 no?
			
			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
			{
				time = ggz->getInt();
				kdDebug(12101) << "Clock set, reading time: " << time << endl;
			}
			if((x >= 0) && (y >= 0) && (x2 >= 0) && (y2 >= 0))
			{
				emit signalMove(QString(i18n("Net: from %1/%2 to %3/%4")).arg(
					QChar(x)).arg(QChar(y)).arg(QChar(x2)).arg(QChar(y2)));
				emit signalDoMove(x - 'A', y - '1', x2 - 'A', y2 - '1');
				// FIXME: this is overhead, merge somewhere
			}
			else
				emit signalMessage(i18n("Invalid move - try again!"));
			break;

		case CHESS_MSG_GAMEOVER:
			kdDebug(12101) << "Got a MSG_GAMEOVER" << endl;

			cval = ggz->getChar();
			kdDebug(12101) << "Gameover msg: " << cval << endl;
			handleGameOver(cval);
			emit signalOver();
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

			emit signalMessage(i18n("Invalid network opcode!"));
			break;
	}
}

void Game::setTime(int timeoption, int time)
{
	if(!ggz) return;

	kdDebug(12101) << "Game::setTime(); timeoption = " << timeoption << ", time = " << time << endl;
	chessInfo.clock_type = timeoption;
	ggz->putChar(CHESS_RSP_TIME);
	if(timeoption) time = (timeoption << 24) + time;
	kdDebug() << "Sent time: " << time << endl;
	ggz->putInt(time);
}

void Game::slotMove(int x, int y, int x2, int y2)
{
	int ret, from, to;

	if(!ggz)
	{
		kdDebug(12101) << "LOCALMOVE: " << x << ", " << y << " => " << x2 << ", " << y2 << endl;
		ret = chess_ai_move(y * 8 + x, y2 * 8 + x2, 0);
		if(ret)
		{
			emit signalMove(QString(i18n("You: from %1/%2 to %3/%4")).arg(
				QChar(x + 'A')).arg(QChar(y + '1')).arg(QChar(x2 + 'A')).arg(QChar(y2 + '1')));
			emit signalDoMove(x, y, x2, y2);

			if(chess_ai_checkmate())
			{
				emit signalMessage(i18n("Game over - you won!"));
				emit signalOver();
				return;
			}

			ret = chess_ai_find(C_BLACK, &from, &to);
			if(ret)
			{
				ret = chess_ai_move(from, to, 0);
				y = from / 8;
				x = from % 8;
				y2 = to / 8;
				x2 = to % 8;
				emit signalMove(QString(i18n("AI: from %1/%2 to %3/%4")).arg(
					QChar(x + 'A')).arg(QChar(y + '1')).arg(QChar(x2 + 'A')).arg(QChar(y2 + '1')));
				emit signalDoMove(x, y, x2, y2);

				if(chess_ai_checkmate())
				{
					emit signalMessage(i18n("Game over - AI won!"));
					emit signalOver();
				}
			}
			else emit signalMessage(i18n("Internal AI error"));
		}
		else emit signalMessage(i18n("Invalid move - try again!"));
		return;
	}

	kdDebug(12101) << "Game::slotMove(); got move: " << x << ", " << y << " => " << x2 << ", " << y2 << endl;
	ggz->putChar(CHESS_REQ_MOVE);
	ggz->putChar(0);ggz->putChar(0);ggz->putChar(0);ggz->putChar(6);// FIXME: string handling
	ggz->putChar(x + 'A');
	ggz->putChar(y + '1');
	ggz->putChar(x2 + 'A');
	ggz->putChar(y2 + '1');
	ggz->putChar(0); // promote value
	ggz->putChar(0);
	emit signalMove(QString(i18n("%1: from %2/%3 to %4/%5")).arg((chessInfo.seat ? i18n("Black") : i18n("White"))).arg(
		QChar(x + 'A')).arg(QChar(y + '1')).arg(QChar(x2 + 'A')).arg(QChar(y2 + '1')));
}

void Game::answerDraw(int draw)
{
	if(!ggz) return;

	if(draw) ggz->putChar(CHESS_REQ_DRAW);
}

void Game::handleGameOver(int cval)
{
	QString s = i18n("Game over! Reason: ");
	switch(cval)
	{
		case CHESS_GAMEOVER_DRAW_AGREEMENT:
			s += i18n("both players agreed on a draw");
			break;
		case CHESS_GAMEOVER_DRAW_STALEMATE:
			s += i18n("stalemate");
			break;
		case CHESS_GAMEOVER_DRAW_POSREP:
			s += i18n("too much repetition of positions");
			break;
		case CHESS_GAMEOVER_DRAW_MATERIAL:
			s += i18n("not enough material");
			break;
		case CHESS_GAMEOVER_DRAW_MOVECOUNT:
			s += i18n("maximum movecount reached");
			break;
		case CHESS_GAMEOVER_DRAW_TIMEMATERIAL:
			s += i18n("time over and no material left");
			break;
		case CHESS_GAMEOVER_WIN_1_MATE:
			s += i18n("player 1 wins by a mate");
			break;
		case CHESS_GAMEOVER_WIN_1_RESIGN:
			s += i18n("player 2 has resigned");
			break;
		case CHESS_GAMEOVER_WIN_1_FLAG:
			s += i18n("player 2 has run out of time");
			break;
		case CHESS_GAMEOVER_WIN_2_MATE:
			s += i18n("player 2 wins by a mate");
			break;
		case CHESS_GAMEOVER_WIN_2_RESIGN:
			s += i18n("player 1 has resigned");
			break;
		case CHESS_GAMEOVER_WIN_2_FLAG:
			s += i18n("player 1 has run out of time");
			break;
		default:
			s += i18n("unknown reason");
	}
	emit signalMessage(s);
}

void Game::handleClock(int cval)
{
	switch(cval)
	{
		case CHESS_CLOCK_NOCLOCK:
			kdDebug(12101) << "This game won't have a time limit." << endl;
			emit signalMessage(i18n("Clock: no limit."));
			break;
		case CHESS_CLOCK_CLIENT:
			kdDebug(12101) << "This game will use a client clock.\nThis option should only be used when playing against people you trust, as it relies much in the client program, that can be cheated.\nSo, if the time behaves very strangely (like your oponnent time never wearing out), he may be running a cheated client.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
			emit signalMessage(i18n("Clock: client-side."));
			break;
		case CHESS_CLOCK_SERVER:
			kdDebug(12101) << "This game will use a server clock.\nIt is very difficult to cheat when using this type of clock, and you should use it if you suspect your oponnent may have a cheated client or if you don't trust him.\nHowever, if either your connection or your opponent's is deeply lagged, it will have a deep effect on the time count as well.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
			emit signalMessage(i18n("Clock: server-side."));
			break;
		case CHESS_CLOCK_SERVERLAG:
			kdDebug(12101) << "This game will use a server clock with lag support.\nIn this option, we will use a server clock, but using a lag meter to compensate for any lag due to Internet connection. Although it's possible to cheat with this option, it is much more difficult then cheating with the client clock.\nBesides, the lag of either connect won't have a so deep effect on the time of the players.\n\nEach player will have " << chessInfo.seconds[0] / 60 << " min : " << chessInfo.seconds[0] % 60 << " sec to win the game." << endl;
			emit signalMessage(i18n("Clock: server-side with lag."));
			break;
		default:
			kdDebug(12101) << "Clock type is " << chessInfo.clock_type << " and time is " << chessInfo.seconds[0] << " seconds." << endl;
			emit signalMessage(i18n("Clock: Error - unexpected type."));
			break;
	}
}

GGZ *Game::getGGZ()
{
	return ggz;
}

