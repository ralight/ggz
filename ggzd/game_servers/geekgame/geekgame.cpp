// Header file
#include "geekgame.h"

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
}

// Spectator data event (ignored)
void Geekgame::spectatorDataEvent(int spectator)
{
}

// Game data event
void Geekgame::dataEvent(int player)
{
	std::cout << "Geekgame: dataEvent" << std::endl;
}

// Error handling event
void Geekgame::errorEvent()
{
	std::cout << "Geekgame: errorEvent" << std::endl;
}

