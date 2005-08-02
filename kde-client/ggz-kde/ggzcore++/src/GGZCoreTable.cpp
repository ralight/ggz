/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// Copyright (C) 2001, 2002 Josef Spillner                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Table //////////////////////////////////////////////////

#include "GGZCoreTable.h"

GGZCoreTable::GGZCoreTable()
{
	m_table = ggzcore_table_new();
	m_destructive = 1;
	m_gametype = NULL;
	m_coregametype = NULL;
}

GGZCoreTable::GGZCoreTable(GGZTable *table)
{
	m_table = table;
	m_destructive = 0;
	m_gametype = NULL;
	m_coregametype = NULL;
}

GGZCoreTable::~GGZCoreTable()
{
	if(m_destructive) ggzcore_table_free(m_table);
}

int GGZCoreTable::init(const GGZGameType* gametype, char* description, const unsigned int seats)
{
	return ggzcore_table_init(m_table, gametype, description, seats);
}

int GGZCoreTable::addPlayer(char* name, const unsigned int seat)
{
	return ggzcore_table_set_seat(m_table, seat, GGZ_SEAT_PLAYER, name);
}

int GGZCoreTable::addBot(char* name, const unsigned int seat)
{
	return ggzcore_table_set_seat(m_table, seat, GGZ_SEAT_BOT, name);
}

int GGZCoreTable::addReserved(char* name, const unsigned int seat)
{
	return ggzcore_table_set_seat(m_table, seat, GGZ_SEAT_RESERVED, name);
}

int GGZCoreTable::removePlayer(char* name)
{
	return ggzcore_table_remove_player(m_table, name);
}

int GGZCoreTable::id()
{
	return ggzcore_table_get_id(m_table);
}

GGZCoreGametype* GGZCoreTable::type()
{
	m_tmpgametype = ggzcore_table_get_type(m_table);
	if(!m_gametype)
	{
		m_gametype = m_tmpgametype;
		m_coregametype = new GGZCoreGametype(m_gametype);
	}
	else
	{
		if(m_gametype != m_tmpgametype)
		{
			delete m_coregametype;
			m_gametype = m_tmpgametype;
			m_coregametype = new GGZCoreGametype(m_gametype);
		}
	}
	return m_coregametype;
}

const char* GGZCoreTable::description()
{
	return ggzcore_table_get_desc(m_table);
}

char GGZCoreTable::state()
{
	return ggzcore_table_get_state(m_table);
}

int GGZCoreTable::countSeats()
{
	return ggzcore_table_get_num_seats(m_table);
}

int GGZCoreTable::countBots()
{
	return ggzcore_table_get_seat_count(m_table, GGZ_SEAT_BOT);
}

int GGZCoreTable::countOpen()
{
	return ggzcore_table_get_seat_count(m_table, GGZ_SEAT_OPEN);
}

const char* GGZCoreTable::playerName(const unsigned int number)
{
	return ggzcore_table_get_nth_player_name(m_table, number);
}

GGZSeatType GGZCoreTable::playerType(const unsigned int number)
{
	return ggzcore_table_get_nth_player_type(m_table, number);
}

GGZTable *GGZCoreTable::table()
{
	return m_table;
}

#ifdef KGGZ_PATCH_SPECTATORS
int GGZCoreTable::addSpectator(char *name, const unsigned int seat)
{
	/*return ggzcore_table_add_spectator(m_table, name, seat);*/
	return 0;
}
#endif

