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

// GGZap includes
#include "ggzap_tray.h"
#include "ggzap_game.h"
#include "ggzap_config.h"

// KDE includes
#include <klocale.h>
#include <kpopupmenu.h>

// Qt includes
#include <qpixmap.h>
#include <qmovie.h>

// System includes
#include "config.h"
#include <iostream>

GGZapTray::GGZapTray(QWidget *parent, const char *name)
: KSystemTray(parent, name)
{
	m_config = NULL;

	m_game = new GGZapGame();

	m_menu = new QPopupMenu(this);

	setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));

	contextMenu()->insertItem(i18n("Configure"), menuconfigure, 1);
	contextMenu()->insertSeparator();
	contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 3);

	connect(contextMenu(), SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu, SIGNAL(activated(int)), SLOT(slotLaunch(int)));

	m_launched = 0;

	show();
}

GGZapTray::~GGZapTray()
{
	delete m_menu;
	delete m_game;
	if(m_config) delete m_config;
}

void GGZapTray::contextMenuAboutToShow(KPopupMenu *menu)
{
	m_menu->clear();

	m_game->clear();
	m_game->autoscan();
	for(int i = 0; i < m_game->count(); i++)
		m_menu->insertItem(QString("%1 (%2)").arg(m_game->name(i)).arg(m_game->frontend(i)), i);
}

void GGZapTray::slotLaunch(int gameid)
{
	emit signalLaunch(m_game->name(gameid), m_game->frontend(gameid));
}

void GGZapTray::slotMenu(int id)
{
	switch(id)
	{
		case menulaunch:
			contextMenu()->removeItem(menulaunch);
			contextMenu()->insertItem(i18n("Cancel game"), menucancel, 3);
			setMovie(QMovie(KGGZ_DIRECTORY "/ggzap/trayradar.mng"));
			break;
		case menucancel:
			emit signalCancel();
			contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 3);
			setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));
			contextMenu()->removeItem(menucancel);
			break;
		case menuconfigure:
			if(!m_config) m_config = new GGZapConfig(NULL, "GGZapConfig");
			m_config->show();
			break;
	}
}

