#include "game.h"
#include "game.moc"

#include <iostream>
#include <kdebug.h>

// quickhack!
#define GGZ_SEAT_OPEN -1

Game::Game(void)
{
	ggz = new GGZ;
	CHECK_PTR(ggz);
	
	chessGame = cgc_create_game();
	cgc_join_game(chessGame, WHITE);
	cgc_join_game(chessGame, BLACK);

	chessInfo.state = CHESS_STATE_INIT;
	chessInfo.clock_type = 0;
	chessInfo.seconds[0] = 0;
	chessInfo.seconds[1] = 0;
	chessInfo.t_seconds[0] = 0;
	chessInfo.t_seconds[1] = 0;
	chessInfo.turn = 0;
	chessInfo.check = FALSE;
	chessInfo.name[0] = "White";
	chessInfo.name[1] = "Black";

	board = new ChessBoard;
	CHECK_PTR(board);
	
	board->show();
}

Game::~Game(void)
{
	if (ggz)
		delete ggz;
	if (board)
		delete board;

	ggz = 0;
	board = 0;
}

void Game::handleNetInput(void)
{
	int opcode, value;
	char cval;
	QString s;

	if ((opcode = ggz->getInt()) < 0)
		return;

	switch (opcode) {
		case CHESS_MSG_SEAT:
			kdDebug() << "Received MSG_SEAT" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;

			chessInfo.seat = ggz->getChar();
			chessInfo.version = ggz->getChar();

			if (chessInfo.version != PROTOCOL_VERSION)
        			kdDebug() << "Incompatible version. The game may not run as expected" << endl;

			break;
		case CHESS_MSG_PLAYERS:
			kdDebug() << "Got an MSG_PLAYERS" << endl;

			if (GGZ_SEAT_OPEN != ggz->getChar())
				if (chessInfo.assign[0] != GGZ_SEAT_OPEN)
					chessInfo.name[0] = ggz->getString();

			if (GGZ_SEAT_OPEN != ggz->getChar())
				if (chessInfo.assign[1] != GGZ_SEAT_OPEN)
					chessInfo.name[1] = ggz->getString();
			break;
		case CHESS_REQ_TIME:
			kdDebug() << "Got an REQ_TIME" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
        			break;
      			
			/* FIXME: Should ask the user for the time */
			break;
		case CHESS_RSP_TIME:
      			kdDebug() << "Got an RSP_TIME" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;

			value = ggz->getInt();
			chessInfo.clock_type = value >> 24;
			chessInfo.seconds[0] = value & 0xFFFFFF;
			chessInfo.seconds[1] = value & 0xFFFFFF;
			chessInfo.t_seconds[0] = value & 0xFFFFFF;
			chessInfo.t_seconds[1] = value & 0xFFFFFF;
			switch(chessInfo.clock_type) {
				case CHESS_CLOCK_NOCLOCK:
					kdDebug() << "This game won't have a time limit." << endl;
					break;
				case CHESS_CLOCK_CLIENT:
					kdDebug() << "This game will use a client clock.\nThis option should only be used when playing against people you trust, as it relies much in the client program, that can be cheated.\nSo, if the time behaves very strangely (like your oponnent time never wearing out), he may be running a cheated client.\n\nEach player will have " << chessInfo.seconds[0]/60 << " min : " << chessInfo.seconds[0]%60 << " sec to win the game." << endl;
					break;
				case CHESS_CLOCK_SERVER:
//					game_popup("This game will use a server clock.\nIt is very difficult to cheat when using this type of clock, and you should use it if you suspect your oponnent may have a cheated client or if you don't trust him.\nHowever, if either your connection or your opponent's is deeply lagged, it will have a deep effect on the time count as well.\n\nEach player will have %d min : %d sec to win the game.", game_info.seconds[0]/60, game_info.seconds[0]%60);
					break;
				case CHESS_CLOCK_SERVERLAG:
//					game_popup("This game will use a server clock with lag support.\nIn this option, we will use a server clock, but using a lag meter to compensate for any lag due to Internet connection. Although it's possible to cheat with this option, it is much more difficult then cheating with the client clock.\nBesides, the lag of either connect won't have a so deep effect on the time of the players.\n\nEach player will have %d min : %d sec to win the game.", game_info.seconds[0]/60, game_info.seconds[0]%60);
					break;
				default:
//					game_popup("Clock type is %d and time is %d", game_info.clock_type, game_info.seconds[0]);
					break;
			}
			break;
		case CHESS_MSG_START:
			kdDebug() << "Got an MSG_START" << endl;

			if (chessInfo.state != CHESS_STATE_WAIT)
				break;
			chessInfo.state = CHESS_STATE_PLAYING;

			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
				chessInfo.timer->changeInterval(1000);

			kdDebug() << "The game has started!" << endl;
			break;

		case CHESS_MSG_MOVE:
			kdDebug() << "Got an MSG_MOVE" << endl;
			s = ggz->getString(6);
			//es_read_string(chess.fd, args, 6);
			/* Should we worry about time ? */
			if (chessInfo.clock_type != CHESS_CLOCK_NOCLOCK)
				//es_read_int(chess.fd, (gint*)args+2);
				ggz->getInt();
//			game_update(CHESS_EVENT_MOVE, args.latin1());
			break;

		case CHESS_MSG_GAMEOVER:
			/* The game is over */
			kdDebug() << "Got a MSG_GAMEOVER" << endl;
			//es_read_char(chess.fd, &args[0]);
			cval = ggz->getChar();
			kdDebug() << "Gameover msg: " << cval << endl;
//			game_update(CHESS_EVENT_GAMEOVER, args);
			break;

		case CHESS_REQ_DRAW:
			/* Do you want to draw the game ? */
			kdDebug() << "Got a REQ_DRAW" << endl;
//			game_update(CHESS_EVENT_DRAW, NULL);
			break;

		case CHESS_RSP_UPDATE:
			/* We want to update the seconds */
			kdDebug() << "Got an RSP_UPDATE" << endl;
			//es_read_int(fd, (int*)args);
			value = ggz->getInt();
			//es_read_int(fd, (int*)args+1);
			value = ggz->getInt();
//			game_update(CHESS_EVENT_UPDATE_TIME, args);
			break;

		default:
			kdDebug() << "Unknown opcode: " << opcode << endl;
			break;
}


}
