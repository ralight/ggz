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

///// GGZ Server /////////////////////////////////////////////////

#include "GGZCoreServer.h"

GGZCoreServer* GGZCoreServer::m_instance = 0;

GGZCoreServer::GGZCoreServer()
{
	m_server = ggzcore_server_new();
	ggzcore_server_reset(m_server);
	m_instance = this;
	m_room = NULL;
	m_coreroom = NULL;
	m_gametypelist = 0;
	m_roomlist = 0;
}

GGZCoreServer::~GGZCoreServer()
{
	ggzcore_server_free(m_server);
	m_instance = 0;
}

int GGZCoreServer::addHook(const GGZCoreServerEvent event, const GGZHookFunc func)
{
	return ggzcore_server_add_event_hook(m_server, (GGZServerEvent)event, func);
}

int GGZCoreServer::addHook(const GGZCoreServerEvent event, const GGZHookFunc func, void* data)
{
	return ggzcore_server_add_event_hook_full(m_server, (GGZServerEvent)event, func, data);
}

int GGZCoreServer::removeHook(const GGZCoreServerEvent event, const GGZHookFunc func)
{
	return ggzcore_server_remove_event_hook(m_server, (GGZServerEvent)event, func);
}

int GGZCoreServer::removeHook(const GGZCoreServerEvent event, const unsigned int id)
{
	return ggzcore_server_remove_event_hook_id(m_server, (GGZServerEvent)event, id);
}


int GGZCoreServer::setHost(const char* host, const unsigned int port)
{
	return ggzcore_server_set_hostinfo(m_server, host, port);
}

int GGZCoreServer::setLogin(const int type, const char* username, const char* password)
{
	return ggzcore_server_set_logininfo(m_server, (GGZLoginType)type, username, password);
}

char* GGZCoreServer::host()
{
	return ggzcore_server_get_host(m_server);
}

int GGZCoreServer::port()
{
	return ggzcore_server_get_port(m_server);
}

GGZLoginType GGZCoreServer::type()
{
	return ggzcore_server_get_type(m_server);
}

char* GGZCoreServer::username()
{
	return ggzcore_server_get_handle(m_server);
}

char* GGZCoreServer::password()
{
	return ggzcore_server_get_password(m_server);
}

int GGZCoreServer::fd()
{
	return ggzcore_server_get_fd(m_server);
}

GGZStateID GGZCoreServer::state()
{
	return ggzcore_server_get_state(m_server);
}

int GGZCoreServer::countRooms()
{
	return ggzcore_server_get_num_rooms(m_server);
}

int GGZCoreServer::countRoomsForce()
{
	int result;

	result = countRooms();

	if(!result)
	{
		listRooms(-1, 0);
		while(!countRooms()) dataRead();
		result = countRooms();
	}

	return result;
}

GGZCoreRoom* GGZCoreServer::room()
{
	m_tmproom = ggzcore_server_get_cur_room(m_server);
	if(!m_room)
	{
		GGZCOREDEBUG("GGZCORE++: room(): create new room\n");
		m_room = m_tmproom;
		m_coreroom = new GGZCoreRoom(m_room);
		m_coreroom->selfRegister(&m_coreroom);
	}
	else
	{
		if(m_room != m_tmproom)
		{
			GGZCOREDEBUG("GGZCORE++: room(): delete room and create new one\n");
			delete m_coreroom;
			GGZCOREDEBUG("GGZCORE++: room(): deleted...\n");
			m_room = m_tmproom;
			m_coreroom = new GGZCoreRoom(m_room);
			m_coreroom->selfRegister(&m_coreroom);
		}
		else
		{
			GGZCOREDEBUG("GGZCORE++: room(): same room\n");
			if(!m_coreroom)
			{
				GGZCOREDEBUG("GGZCORE++: room(): Hey Joe, that's unfair! Thou deleted my child!\n");
				m_coreroom = new GGZCoreRoom(m_room);
				m_coreroom->selfRegister(&m_coreroom);
			}
		}
	}
	return m_coreroom;
}

GGZCoreRoom* GGZCoreServer::room(const unsigned int number)
{
	m_tmproom = ggzcore_server_get_nth_room(m_server, number);
	if(!m_room)
	{
		GGZCOREDEBUG("GGZCORE++: room(number): create new room\n");
		m_room = m_tmproom;
		m_coreroom = new GGZCoreRoom(m_room);
		m_coreroom->selfRegister(&m_coreroom);
	}
	else
	{
		if(m_room != m_tmproom)
		{
			GGZCOREDEBUG("GGZCORE++: room(number): delete room and create new one\n");
			delete m_coreroom;
			m_room = m_tmproom;
			m_coreroom = new GGZCoreRoom(m_room);
			m_coreroom->selfRegister(&m_coreroom);
		}
		else
		{
			GGZCOREDEBUG("GGZCORE++: room(number): same room\n");
			if(!m_coreroom)
			{
				GGZCOREDEBUG("GGZCORE++: room(): Hey Joe, that's unfair! Thou deleted my child!\n");
				m_coreroom = new GGZCoreRoom(m_room);
				m_coreroom->selfRegister(&m_coreroom);
			}
		}
	}
	return m_coreroom;
}

int GGZCoreServer::joinRoom(const unsigned int number)
{
	return ggzcore_server_join_room(m_server, number);
}

int GGZCoreServer::countGames()
{
	return ggzcore_server_get_num_gametypes(m_server);
}

GGZGameType* GGZCoreServer::game(const unsigned int number)
{
	return ggzcore_server_get_nth_gametype(m_server, number);
}

int GGZCoreServer::isOnline()
{
	return ggzcore_server_is_online(m_server);
}

int GGZCoreServer::isLoggedIn()
{
	return ggzcore_server_is_logged_in(m_server);
}

int GGZCoreServer::isInRoom()
{
	return ggzcore_server_is_in_room(m_server);
}

int GGZCoreServer::isAtTable()
{
	return ggzcore_server_is_at_table(m_server);
}

int GGZCoreServer::connect()
{
	int ret;
	GGZCOREDEBUG("connect start.\n");
	ret = ggzcore_server_connect(m_server);
	GGZCOREDEBUG("connect done.\n");
	return ret;
}

int GGZCoreServer::login()
{
	return ggzcore_server_login(m_server);
}

int GGZCoreServer::motd()
{
	return ggzcore_server_motd(m_server);
}

int GGZCoreServer::logout()
{
	return ggzcore_server_logout(m_server);
}

int GGZCoreServer::listRooms(const int type, const char verbose)
{
	if(m_roomlist) return 1;
	m_roomlist = ggzcore_server_list_rooms(m_server, type, verbose);
	return m_roomlist;
}

int GGZCoreServer::listGames(const char verbose)
{
	if(m_gametypelist) return 1;
	m_gametypelist = ggzcore_server_list_gametypes(m_server, verbose);
	return m_gametypelist;
}

int GGZCoreServer::dataPending()
{
	return ggzcore_server_data_is_pending(m_server);
}

int GGZCoreServer::dataRead()
{
	return ggzcore_server_read_data(m_server);
}

int GGZCoreServer::dataWrite()
{
	return ggzcore_server_write_data(m_server);
}

GGZCoreServer* GGZCoreServer::instance()
{
	return m_instance;
}

GGZServer *GGZCoreServer::server()
{
	return m_server;
}

void GGZCoreServer::resetRoom()
{
	m_coreroom = NULL;
}

void GGZCoreServer::rescue()
{
	if(!m_server) return;
	ggzcore_server_free(m_server);
	m_server = NULL;
}

int GGZCoreServer::logSession(const char *filename)
{
	return ggzcore_server_log_session(m_server, filename);
}

