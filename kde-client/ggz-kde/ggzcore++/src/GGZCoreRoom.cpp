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

///// GGZ Room ///////////////////////////////////////////////////

#include "GGZCoreRoom.h"
#include "GGZCoreGametype.h"
#include "GGZCoreTable.h"
#include "GGZCorePlayer.h"
#include "config.h"

GGZCoreRoom::GGZCoreRoom()
{
	m_room = ggzcore_room_new();
	m_destructive = 1;
	m_gametype = NULL;
	m_coregametype = NULL;
	m_table = NULL;
	m_player = NULL;
	m_coretable = NULL;
	m_register = NULL;
}

GGZCoreRoom::GGZCoreRoom(GGZRoom* room)
{
	m_room = room;
	m_destructive = 0;
	m_gametype = NULL;
	m_coregametype = NULL;
	m_table = NULL;
	m_player = NULL;
	m_coretable = NULL;
	m_register = NULL;
}

GGZCoreRoom::~GGZCoreRoom()
{
	if(m_destructive)
	{
		GGZCOREDEBUG("GGZCORE++: ~GGZCoreRoom(): DESTROY!\n");
		ggzcore_room_free(m_room);
	}
	if(m_register) *m_register = NULL;
}

int GGZCoreRoom::addHook(const GGZCoreRoomEvent event, const GGZHookFunc func)
{
	return ggzcore_room_add_event_hook(m_room, (GGZRoomEvent)event, func);
}

int GGZCoreRoom::addHook(const GGZCoreRoomEvent event, const GGZHookFunc func, void* data)
{
	return ggzcore_room_add_event_hook_full(m_room, (GGZRoomEvent)event, func, data);
}

int GGZCoreRoom::removeHook(const GGZCoreRoomEvent event, const GGZHookFunc func)
{
	return ggzcore_room_remove_event_hook(m_room, (GGZRoomEvent)event, func);
}

int GGZCoreRoom::removeHook(const GGZCoreRoomEvent event, const unsigned int id)
{
	return ggzcore_room_remove_event_hook_id(m_room, (GGZRoomEvent)event, id);
}

int GGZCoreRoom::init(GGZServer* server, const unsigned int id, const char* name, const unsigned int game, const char* description, const char *category)
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_room_init(m_room, server, id, name, game, description, category);
#else
	return ggzcore_room_init(m_room, server, id, name, game, description);
#endif
}

char* GGZCoreRoom::name()
{
	return ggzcore_room_get_name(m_room);
}

char* GGZCoreRoom::description()
{
	return ggzcore_room_get_desc(m_room);
}

char* GGZCoreRoom::category()
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_room_get_category(m_room);
#else
	return "";
#endif
}

GGZCoreGametype* GGZCoreRoom::gametype()
{
	m_tmpgametype = ggzcore_room_get_gametype(m_room);
	if(!m_gametype)
	{
		m_gametype = m_tmpgametype;
		if(!m_gametype) GGZCOREDEBUG("isNull()!!!\n");
		m_coregametype = new GGZCoreGametype(m_gametype);
	}
	else
	{
		if(m_gametype != m_tmpgametype)
		{
			GGZCOREDEBUG("GGZCORE++: gametype(): delete and recreate gametype (%p)\n", m_tmpgametype);
			delete m_coregametype;
			m_gametype = m_tmpgametype;
			m_coregametype = new GGZCoreGametype(m_gametype);
		}
	}
	return m_coregametype;
}

int GGZCoreRoom::countPlayers()
{
	return ggzcore_room_get_num_players(m_room);
}

GGZCorePlayer* GGZCoreRoom::player(const unsigned int number)
{
	m_tmpplayer = ggzcore_room_get_nth_player(m_room, number);
	if(!m_player)
	{
		m_player = m_tmpplayer;
		m_coreplayer = new GGZCorePlayer(m_player);
	}
	else
	{
		if(m_player != m_tmpplayer)
		{
			delete m_coreplayer;
			m_player = m_tmpplayer;
			m_coreplayer = new GGZCorePlayer(m_player);
		}
	}
	return m_coreplayer;
}

int GGZCoreRoom::countTables()
{
	return ggzcore_room_get_num_tables(m_room);
}

GGZCoreTable* GGZCoreRoom::table(const unsigned int number)
{
	m_tmptable = ggzcore_room_get_nth_table(m_room, number);
	if(!m_table)
	{
		GGZCOREDEBUG("GGZCORE++: table(number): create new table\n");
		m_table = m_tmptable;
		m_coretable = new GGZCoreTable(m_table);
	}
	else
	{
		if(m_table != m_tmptable)
		{
			GGZCOREDEBUG("GGZCORE++: table(number): delete and recreate table\n");
			delete m_coretable;
			m_table = m_tmptable;
			m_coretable = new GGZCoreTable(m_table);
		}
		else GGZCOREDEBUG("GGZCORE++: table(number): same\n");
	}
	return m_coretable;
}

int GGZCoreRoom::listPlayers()
{
	return ggzcore_room_list_players(m_room);
}

int GGZCoreRoom::listTables(const int type, const char global)
{
	return ggzcore_room_list_tables(m_room, type, global);
}

int GGZCoreRoom::chat(const GGZChatType opcode, const char* player, const char* message)
{
	return ggzcore_room_chat(m_room, opcode, player, message);
}

int GGZCoreRoom::launchTable(GGZTable* table)
{
	return ggzcore_room_launch_table(m_room, table);
}

int GGZCoreRoom::joinTable(const unsigned int number)
{
	return ggzcore_room_join_table(m_room, number, 0);
}

int GGZCoreRoom::joinTableSpectator(const unsigned int number)
{
#ifdef KGGZ_PATCH_SPECTATORS
	return ggzcore_room_join_table(m_room, number, 1);
#else
	return 0;
#endif
}

int GGZCoreRoom::leaveTable(int force)
{
	return ggzcore_room_leave_table(m_room, force);
}

int GGZCoreRoom::leaveTableSpectator()
{
#ifdef KGGZ_PATCH_SPECTATORS
	return ggzcore_room_leave_table(m_room, 0);
#else
	return 0;
#endif
}

int GGZCoreRoom::sendData(char* buffer)
{
	//return ggzcore_room_send_game_data(m_room, buffer);
	return 0;
}

GGZRoom* GGZCoreRoom::room()
{
	return m_room;
}

void GGZCoreRoom::selfRegister(GGZCoreRoom **room)
{
	m_register = room;
}

