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

///// GGZ Game ///////////////////////////////////////////////////

#include "GGZCoreGame.h"

GGZCoreGame::GGZCoreGame()
{
	m_game = ggzcore_game_new();
}

GGZCoreGame::~GGZCoreGame()
{
	ggzcore_game_free(m_game);
}

int GGZCoreGame::addHook(const GGZCoreGameEvent event, const GGZHookFunc func)
{
	return ggzcore_game_add_event_hook(m_game, (GGZGameEvent)event, func);
}

int GGZCoreGame::addHook(const GGZCoreGameEvent event, const GGZHookFunc func, void* data)
{
	return ggzcore_game_add_event_hook_full(m_game, (GGZGameEvent)event, func, data);
}

int GGZCoreGame::removeHook(const GGZCoreGameEvent event, const GGZHookFunc func)
{
	return ggzcore_game_remove_event_hook(m_game, (GGZGameEvent)event, func);
}

int GGZCoreGame::removeHook(const GGZCoreGameEvent event, const unsigned int id)
{
	return ggzcore_game_remove_event_hook_id(m_game, (GGZGameEvent)event, id);
}

int GGZCoreGame::init(GGZModule* module)
{
	return ggzcore_game_init(m_game, module);
}

int GGZCoreGame::fd()
{
	return ggzcore_game_get_fd(m_game);
}

GGZModule* GGZCoreGame::module()
{
	return ggzcore_game_get_module(m_game);
}

int GGZCoreGame::launch()
{
	return ggzcore_game_launch(m_game);
}

int GGZCoreGame::join()
{
	return ggzcore_game_join(m_game);
}

int GGZCoreGame::dataSend(char* buffer)
{
	return ggzcore_game_send_data(m_game, buffer);
}

int GGZCoreGame::dataPending()
{
	return ggzcore_game_data_is_pending(m_game);
}

int GGZCoreGame::dataRead()
{
	return ggzcore_game_read_data(m_game);
}

int GGZCoreGame::dataWrite()
{
	return ggzcore_game_write_data(m_game);
}
