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

///// GGZ GameType ///////////////////////////////////////////////

#include "GGZCoreGametype.h"

GGZCoreGametype::GGZCoreGametype(GGZGameType *gametype)
{
	m_gametype = gametype;
}

GGZCoreGametype::GGZCoreGametype()
{
	m_gametype = 0;
}

GGZCoreGametype::~GGZCoreGametype()
{
}

char* GGZCoreGametype::name()
{
	return ggzcore_gametype_get_name(m_gametype);
}

char* GGZCoreGametype::protocol()
{
	return ggzcore_gametype_get_protocol(m_gametype);
}

char* GGZCoreGametype::version()
{
	return ggzcore_gametype_get_version(m_gametype);
}

char* GGZCoreGametype::author()
{
	return ggzcore_gametype_get_author(m_gametype);
}

char* GGZCoreGametype::url()
{
	return ggzcore_gametype_get_url(m_gametype);
}

char* GGZCoreGametype::description()
{
	return ggzcore_gametype_get_desc(m_gametype);
}

int GGZCoreGametype::maxPlayers()
{
	return ggzcore_gametype_get_max_players(m_gametype);
}

int GGZCoreGametype::maxBots()
{
	return ggzcore_gametype_get_max_bots(m_gametype);
}

int GGZCoreGametype::isPlayersValid(unsigned int number)
{
	return ggzcore_gametype_num_players_is_valid(m_gametype, number);
}

int GGZCoreGametype::isBotsValid(unsigned int number)
{
	return ggzcore_gametype_num_players_is_valid(m_gametype, number);
}

GGZGameType *GGZCoreGametype::gametype()
{
	return m_gametype;
}
