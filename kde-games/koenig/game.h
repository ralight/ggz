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

#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include <qtimer.h>

#include "board.h"
#include "ggz.h"
#include "cgc.h"
#include "chess.h"

typedef struct {
	char clock_type;
	int seconds[2];
	int t_seconds[2];
	char state;
	int turn;
	char seat;
	char version;
	char assign[2];
	QString name[2];
	QPoint dest;
	QPoint src;
	char check;
	QTimer *timer;
} GameInfo;

class Game : public QObject
{
	Q_OBJECT
public:
	Game(void);
	~Game(void);
	//void handleGameUpdate();
	void setTime(int timeoption, int time); // FIXME: consistent naming like answerTime() !!!
	void answerDraw(int draw);
	void handleGameOver(int cval);
	void handleClock(int cval);

public slots:
	void handleNetInput(void);
	void slotMove(int x, int y, int x2, int y2);

signals:
	void signalStart(int seat);
	void signalNewGame();
	void signalMessage(QString msg);
	void signalMove(QString msg);
	void signalDoMove(int x, int y, int x2, int y2);
	void signalDraw();

private:
	GGZ *ggz;
	//ChessBoard *board;
	GameInfo chessInfo;
	game_t *chessGame;
	struct chess_info chess;
};

#define PROTOCOL_VERSION 6

/* Definition of states */
#define CHESS_STATE_INIT 0
#define CHESS_STATE_WAIT 1
#define CHESS_STATE_PLAYING 2
#define CHESS_STATE_DONE 3

/* Definition of messages */
#define CHESS_MSG_SEAT 1
#define CHESS_MSG_PLAYERS 2
#define CHESS_REQ_TIME 3
#define CHESS_RSP_TIME 4
#define CHESS_MSG_START 5
#define CHESS_REQ_MOVE 6
#define CHESS_MSG_MOVE 7
#define CHESS_MSG_GAMEOVER 8
#define CHESS_REQ_UPDATE 9
#define CHESS_RSP_UPDATE 10
#define CHESS_MSG_UPDATE 11
#define CHESS_REQ_FLAG 12
#define CHESS_REQ_DRAW 13

/* Clock types */
#define CHESS_CLOCK_NOCLOCK 0
#define CHESS_CLOCK_CLIENT 1
#define CHESS_CLOCK_SERVERLAG 2
#define CHESS_CLOCK_SERVER 3

/* Game over types */
#define CHESS_GAMEOVER_DRAW_AGREEMENT 1
#define CHESS_GAMEOVER_DRAW_STALEMATE 2
#define CHESS_GAMEOVER_DRAW_POSREP 3
#define CHESS_GAMEOVER_DRAW_MATERIAL 4
#define CHESS_GAMEOVER_DRAW_MOVECOUNT 5
#define CHESS_GAMEOVER_DRAW_TIMEMATERIAL 6
#define CHESS_GAMEOVER_WIN_1_MATE 7
#define CHESS_GAMEOVER_WIN_1_RESIGN 8
#define CHESS_GAMEOVER_WIN_1_FLAG 9
#define CHESS_GAMEOVER_WIN_2_MATE 10
#define CHESS_GAMEOVER_WIN_2_RESIGN 11
#define CHESS_GAMEOVER_WIN_2_FLAG 12

#endif
