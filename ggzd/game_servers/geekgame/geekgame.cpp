// Header file
#include "geekgame.h"

// Geekgame includes
#include "protocol.h"

// GGZ includes
#include <ggz.h>

// System includes
#include <iostream>

// Constructor: inherit from ggzgameserver
Geekgame::Geekgame()
: GGZGameServer()
{
}

// Destructor
Geekgame::~Geekgame()
{
}

// State change hook
void Geekgame::stateEvent()
{
	std::cout << "Geekgame: stateEvent" << std::endl;
}

// Player join hook
void Geekgame::joinEvent(int player)
{
	std::cout << "Geekgame: joinEvent" << std::endl;

	// Send greeter
	int channel = fd(player);
	ggz_write_char(channel, op_server_greeting);
	ggz_write_string(channel, "geekgame");
	ggz_write_int(channel, 1);
}

// Player leave event
void Geekgame::leaveEvent(int player)
{
	std::cout << "Geekgame: leaveEvent" << std::endl;
}

// Spectator join event
void Geekgame::spectatorJoinEvent(int spectator)
{
	std::cout << "Geekgame: spectatorJoinEvent" << std::endl;
}

// Spectator leave event
void Geekgame::spectatorLeaveEvent(int spectator)
{
	std::cout << "Geekgame: spectatorLeaveEvent" << std::endl;

	// Stop the game for the time being
	game_stop();
}

// Spectator data event (ignored)
void Geekgame::spectatorDataEvent(int spectator)
{
}

// Game data event
void Geekgame::dataEvent(int player)
{
	char opcode;
	char playername[256], playerpic[256];

	std::cout << "Geekgame: dataEvent" << std::endl;

	// Read data
	int channel = fd(player);

	ggz_read_char(channel, &opcode);
	switch(opcode)
	{
		case op_client_presentation:
			ggz_read_string(channel, playername, 256);
			ggz_read_string(channel, playerpic, 256);

			// Broadcast
			for(int i = 0; i < players(); i++)
			{
				int bfd = fd(i);
				ggz_write_char(bfd, op_server_newplayer);
				ggz_write_string(bfd, playername);
				ggz_write_string(bfd, playerpic);
			}

			// If ready, announce game start
			if(0 == 0) game_start();
			break;
		default:
			// Discard
			break;
	}
}

// Error handling event
void Geekgame::errorEvent()
{
	std::cout << "Geekgame: errorEvent" << std::endl;
}

void Geekgame::game_start()
{
	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		ggz_write_char(bfd, op_server_gamestart);
	}
}

void Geekgame::game_stop()
{
	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		ggz_write_char(bfd, op_server_gamestop);
	}
}

void Geekgame::game_end()
{
	for(int i = 0; i < players(); i++)
	{
		int bfd = fd(i);
		ggz_write_char(bfd, op_server_gameend);
		//ggz_write_int(bfd, winner);
	}
}

