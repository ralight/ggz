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

///// GGZ GameType ///////////////////////////////////////////////

#include "GGZCoreGametype.h"
#include "config.h"

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

char* GGZCoreGametype::protocolVersion()
{
	return ggzcore_gametype_get_prot_version(m_gametype);
}

char *GGZCoreGametype::protocolEngine()
{
	return ggzcore_gametype_get_prot_engine(m_gametype);
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

int GGZCoreGametype::maxSpectators()
{
#ifdef KGGZ_PATCH_SPECTATORS
	return ggzcore_gametype_get_spectators_allowed(m_gametype);
#else
	return 0;
#endif
}

int GGZCoreGametype::isPlayersValid(unsigned int number)
{
	return ggzcore_gametype_num_players_is_valid(m_gametype, number);
}

int GGZCoreGametype::isBotsValid(unsigned int number)
{
	return ggzcore_gametype_num_players_is_valid(m_gametype, number);
}

int GGZCoreGametype::isSpectatorsValid(unsigned int number)
{
#ifdef KGGZ_PATCH_SPECTATORS
	/*return ggzcore_gametype_num_spectators_is_valid(m_gametype, number);*/
	return 1;
#else
	return 1;
#endif
}

GGZGameType *GGZCoreGametype::gametype()
{
	return m_gametype;
}

char* GGZCoreGametype::category()
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_gametype_get_category(m_gametype);
#else
	return "";
#endif
}

char* GGZCoreGametype::rating()
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_gametype_get_rating(m_gametype);
#else
	return "";
#endif
}

