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

///// GGZ Player /////////////////////////////////////////////////

#include "GGZCorePlayer.h"

GGZCorePlayer::GGZCorePlayer()
{
}

GGZCorePlayer::~GGZCorePlayer()
{
}

char* GGZCorePlayer::name()
{
	return ggzcore_player_get_name(m_player);
}

GGZTable* GGZCorePlayer::table()
{
	return ggzcore_player_get_table(m_player);
}

int GGZCorePlayer::lag()
{
	return ggzcore_player_get_lag(m_player);
}

int GGZCorePlayer::type()
{
	return ggzcore_player_get_type(m_player);
}

