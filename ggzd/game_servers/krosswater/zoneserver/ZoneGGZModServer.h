#ifndef ZONE_GGZ_MOD_SERVER_H
#define ZONE_GGZ_MOD_SERVER_H

#include "ZoneGGZ.h"

class ZoneGGZModServer
{
public:
	ZoneGGZModServer();
	~ZoneGGZModServer();
	virtual int slotZoneInput(int fd, int i);
	virtual void slotZoneAI();
	int zoneMainLoop();
	void ZoneRegister(char* gamename, int gamemode, int maxplayers);
	void zoneNextTurn();

	int zoneTurn();

	int m_maxplayers;
	int m_numplayers;

private:
	int game_send_seat(int seat);
	int game_send_players();
	void game_send_rules();
	int game_input(int zone_fd, int *p_fd);
	int game_update(int event, void* data);

	int m_gamemode;
	int* m_players;
	int m_turn;
	int m_state;
	char* m_gamename;
	int ggz_sock;
	int m_ready;
};

#endif
