#ifndef GEEKGAME_SERVER_H
#define GEEKGAME_SERVER_H

// GGZ includes
#include "ggzgameserver.h"

// Definitions
#define ARRAY_WIDTH 20
#define ARRAY_HEIGHT 10

// Geekgame server object
class Geekgame : public GGZGameServer
{
	public:
		Geekgame();
		~Geekgame();
		void stateEvent();
		void joinEvent(int player);
		void leaveEvent(int player);
		void spectatorJoinEvent(int spectator);
		void spectatorLeaveEvent(int spectator);
		void spectatorDataEvent(int spectator);
		void dataEvent(int player);
		void errorEvent();

	private:
		void game_start();
		void game_stop();
		void game_end();

		int map_check(int x, int y);

		int m_players;
		int m_valid[2];
		int m_array[ARRAY_WIDTH][ARRAY_HEIGHT];
		int m_mode;
};

#endif

