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
#include "GGZCoreCommon.h"

#include "config.h"

GGZCoreGametype::GGZCoreGametype(const GGZGameType *gametype)
{
	m_gametype = gametype;
}

GGZCoreGametype::GGZCoreGametype()
{
	m_gametype = NULL;
}

GGZCoreGametype::~GGZCoreGametype()
{
}

const char* GGZCoreGametype::name()
{
	return ggzcore_gametype_get_name(m_gametype);
}

const char* GGZCoreGametype::protocolVersion()
{
	return ggzcore_gametype_get_prot_version(m_gametype);
}

const char *GGZCoreGametype::protocolEngine()
{
	return ggzcore_gametype_get_prot_engine(m_gametype);
}

const char* GGZCoreGametype::version()
{
	return ggzcore_gametype_get_version(m_gametype);
}

const char* GGZCoreGametype::author()
{
	return ggzcore_gametype_get_author(m_gametype);
}

const char* GGZCoreGametype::url()
{
	return ggzcore_gametype_get_url(m_gametype);
}

const char* GGZCoreGametype::description()
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

const GGZGameType *GGZCoreGametype::gametype()
{
	return m_gametype;
}

const char* GGZCoreGametype::category()
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_gametype_get_category(m_gametype);
#else
	return "";
#endif
}

const char* GGZCoreGametype::rating()
{
#ifdef KGGZ_PATCH_C_AND_R
	return ggzcore_gametype_get_rating(m_gametype);
#else
	return "";
#endif
}

int GGZCoreGametype::namedBots()
{
	return ggzcore_gametype_get_num_namedbots(m_gametype);
}

const char *GGZCoreGametype::namedBotName(unsigned int number)
{
	return ggzcore_gametype_get_namedbot_name(m_gametype, number);
}

const char *GGZCoreGametype::namedBotClass(unsigned int number)
{
	return ggzcore_gametype_get_namedbot_class(m_gametype, number);
}

