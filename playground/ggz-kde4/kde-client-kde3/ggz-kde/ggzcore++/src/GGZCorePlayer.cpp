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
	int record, dummy;

	(void) ggzcore_player_get_record(m_player, &record, &dummy, &dummy, &dummy);
	return record;
}

int GGZCorePlayer::recordLosses()
{
	int record, dummy;

	(void)ggzcore_player_get_record(m_player, &dummy, &record, &dummy, &dummy);
	return record;
}

int GGZCorePlayer::recordTies()
{
	int record, dummy;

	(void)ggzcore_player_get_record(m_player, &dummy, &dummy, &record, &dummy);
	return record;
}

int GGZCorePlayer::recordForfeits()
{
	int record, dummy;

	(void)ggzcore_player_get_record(m_player, &dummy, &dummy, &dummy, &record);
	return record;
}

int GGZCorePlayer::rating()
{
	int rating;

	(void)ggzcore_player_get_rating(m_player, &rating);
	return rating;
}

int GGZCorePlayer::ranking()
{
	int ranking;

	(void)ggzcore_player_get_ranking(m_player, &ranking);
	return ranking;
}

int GGZCorePlayer::highscore()
{
	int highscore;

	(void)ggzcore_player_get_highscore(m_player, &highscore);
	return highscore;
}

bool GGZCorePlayer::hasRecord()
{
	int record, ret, dummy;

	ret = ggzcore_player_get_record(m_player, &record, &dummy, &dummy, &dummy);
	if(ret) return true;
	return false;
}

bool GGZCorePlayer::hasRating()
{
	int rating;
	int ret;

	ret = ggzcore_player_get_rating(m_player, &rating);
	if(ret) return true;
	return false;
}

bool GGZCorePlayer::hasRanking()
{
	int ranking;
	int ret;

	ret = ggzcore_player_get_ranking(m_player, &ranking);
	if(ret) return true;
	return false;
}

bool GGZCorePlayer::hasHighscore()
{
	int highscore;
	int ret;

	ret = ggzcore_player_get_highscore(m_player, &highscore);
	if(ret) return true;
	return false;
}

bool GGZCorePlayer::hasPermission(PlayerPermission permission)
{
	return ggzcore_player_has_perm(m_player, (GGZPerm)permission);
}

bool GGZCorePlayer::grantPermission(PlayerPermission permission)
{
	return ggzcore_player_set_perm(m_player, (GGZPerm)permission, 1);
}

bool GGZCorePlayer::revokePermission(PlayerPermission permission)
{
	return ggzcore_player_set_perm(m_player, (GGZPerm)permission, 0);
}

