/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// (C) 2001 Josef Spillner                                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Room ///////////////////////////////////////////////////

#include "GGZCoreRoom.h"
#include "config.h"

GGZCoreRoom::GGZCoreRoom()
{
	m_room = ggzcore_room_new();
	m_destructive = 1;
	m_gametype = NULL;
	m_coregametype = NULL;
	m_table = NULL;
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
	m_coretable = NULL;
	m_register = NULL;
}

GGZCoreRoom::~GGZCoreRoom()
{
	GGZCOREDEBUG("GGZCORE++: ~GGZCoreRoom(): m_destructive is %i\n", m_destructive);
	if(m_destructive)
	{
		GGZCOREDEBUG("GGZCORE++: ~GGZCoreRoom(): DESTROY!\n");
		ggzcore_room_free(m_room);
	}
	if(m_register)
	{
		GGZCOREDEBUG("Unregister at: %i -> %i\n", m_register, *m_register);
		*m_register = NULL;
	}
	GGZCOREDEBUG("GGZCORE++: ~GGZCoreRoom(): ready!\n");
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

int GGZCoreRoom::init(const GGZServer* server, const unsigned int id, const char* name, const unsigned int game, const char* description, const char *category)
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
		GGZCOREDEBUG("GGZCORE++: gametype(): create new gametype\n");
		m_gametype = m_tmpgametype;
		if(!m_gametype) GGZCOREDEBUG("isNull()!!!\n");
		m_coregametype = new GGZCoreGametype(m_gametype);
	}
	else
	{
		if(m_gametype != m_tmpgametype)
		{
			GGZCOREDEBUG("GGZCORE++: gametype(): delete and recreate gametype (%i)\n", m_tmpgametype);
			delete m_coregametype;
			m_gametype = m_tmpgametype;
			m_coregametype = new GGZCoreGametype(m_gametype);
		}
		else GGZCOREDEBUG("GGZCORE++: gametype(): same\n");
	}
	return m_coregametype;
}

int GGZCoreRoom::countPlayers()
{
	return ggzcore_room_get_num_players(m_room);
}

GGZPlayer* GGZCoreRoom::player(const unsigned int number)
{
	return ggzcore_room_get_nth_player(m_room, number);
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

int GGZCoreRoom::chat(const GGZChatOp opcode, const char* player, const char* message)
{
	return ggzcore_room_chat(m_room, opcode, player, message);
}

int GGZCoreRoom::launchTable(GGZTable* table)
{
	return ggzcore_room_launch_table(m_room, table);
}

int GGZCoreRoom::joinTable(const unsigned int number)
{
	return ggzcore_room_join_table(m_room, number);
}

int GGZCoreRoom::leaveTable()
{
	return ggzcore_room_leave_table(m_room);
}

int GGZCoreRoom::sendData(char* buffer)
{
	return ggzcore_room_send_game_data(m_room, buffer);
}

GGZRoom* GGZCoreRoom::room()
{
	return m_room;
}

void GGZCoreRoom::selfRegister(GGZCoreRoom **room)
{
	m_register = room;
	GGZCOREDEBUG("Register at: %i -> %i\n", m_register, *m_register);
}
