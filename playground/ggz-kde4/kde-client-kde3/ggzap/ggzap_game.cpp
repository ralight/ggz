// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "ggzap_game.h"

// KDE includes
#include <kstandarddirs.h>
#include <ksimpleconfig.h>

// Configuration
#include "config.h"

GGZapGame::GGZapGame()
{
}

GGZapGame::~GGZapGame()
{
	clear();
}

void GGZapGame::addGame(QString name, QString frontend)
{
	m_gamelist << name;
	m_frontendlist << frontend;
}

int GGZapGame::count()
{
	return m_gamelist.count();
}

QString GGZapGame::name(int id)
{
	if((id < 0) || (id >= (int)m_gamelist.count())) return QString::null;
	return *(m_gamelist.at(id));
}

QString GGZapGame::frontend(int id)
{
	if((id < 0) || (id >= (int)m_frontendlist.count())) return QString::null;
	return *(m_frontendlist.at(id));
}

void GGZapGame::clear()
{
	m_gamelist.clear();
	m_frontendlist.clear();
}

void GGZapGame::autoscan()
{
	QStringList enginelist, gameslist;
	KSimpleConfig conf(GGZMODULECONFDIR "/ggz.modules");

	conf.setGroup("Games");
	QString engine = conf.readEntry("*Engines*");
	enginelist = enginelist.split(" ", engine);
	for(QStringList::Iterator it = enginelist.begin(); it != enginelist.end(); it++)
	{
		conf.setGroup("Games");
		QString game = conf.readEntry((*it));
		gameslist = gameslist.split(" ", game);
		for(QStringList::Iterator it2 = gameslist.begin(); it2 != gameslist.end(); it2++)
		{
			conf.setGroup((*it2));
			QString frontend = conf.readEntry("Frontend");
			QString gamename = conf.readEntry("Name");

			addGame(gamename, frontend);
		}
	}
}

