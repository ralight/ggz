#ifndef GEEKGAME_SERVER_H
#define GEEKGAME_SERVER_H

// GGZ includes
#include "ggzgameserver.h"

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

		int m_players;
		int m_valid[2];
};

#endif

