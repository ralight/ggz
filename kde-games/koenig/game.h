#ifndef _game_h
#define _game_h

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

/* Definition of events */
/*#define CHESS_EVENT_SEAT 0
#define CHESS_EVENT_PLAYERS 1
#define CHESS_EVENT_QUIT 2
#define CHESS_EVENT_INIT 3
#define CHESS_EVENT_MOVE_END 4
#define CHESS_EVENT_MOVE 5
#define CHESS_EVENT_TIME_REQUEST 6
#define CHESS_EVENT_TIME_OPTION 7
#define CHESS_EVENT_GAMEOVER 8
#define CHESS_EVENT_START 9
#define CHESS_EVENT_UPDATE_TIME 10
#define CHESS_EVENT_REQUEST_UPDATE 11
#define CHESS_EVENT_FLAG 12
#define CHESS_EVENT_DRAW 13*/

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
