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

GGZCorePlayer::GGZCorePlayer(GGZPlayer *player)
{
	m_player = player;
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

int GGZCorePlayer::recordWins()
{
	int record, ret, dummy;

	ret = ggzcore_player_get_record(m_player, &record, &dummy, &dummy, &dummy);
	if(ret) return record;
	return 0;
}

int GGZCorePlayer::recordLosses()
{
	int record, ret, dummy;

	ret = ggzcore_player_get_record(m_player, &dummy, &record, &dummy, &dummy);
	if(ret) return record;
	return 0;
}

int GGZCorePlayer::recordTies()
{
	int record, ret, dummy;

	ret = ggzcore_player_get_record(m_player, &dummy, &dummy, &record, &dummy);
	if(ret) return record;
	return 0;
}

int GGZCorePlayer::recordForfeits()
{
	int record, ret, dummy;

	ret = ggzcore_player_get_record(m_player, &dummy, &dummy, &dummy, &record);
	if(ret) return record;
	return 0;
}

int GGZCorePlayer::rating()
{
	int rating;
	int ret;

	ret = ggzcore_player_get_rating(m_player, &rating);
	if(ret) return rating;
	return 0;
}

int GGZCorePlayer::ranking()
{
	int ranking;
	int ret;

	ret = ggzcore_player_get_ranking(m_player, &ranking);
	if(ret) return ranking;
	return 0;
}

long GGZCorePlayer::highscore()
{
	long highscore;
	int ret;

	ret = ggzcore_player_get_highscore(m_player, &highscore);
	if(ret) return highscore;
	return 0;
}

