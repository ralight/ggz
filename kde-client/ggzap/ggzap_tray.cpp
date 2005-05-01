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
#include <kapplication.h>
#include <kaboutapplication.h>
#include <kiconloader.h>

// Qt includes
#include <qpixmap.h>
#include <qmovie.h>
#include <qtooltip.h>

// System includes
#include "config.h"
#include <iostream>

GGZapTray::GGZapTray(QWidget *parent, const char *name)
: KSystemTray(parent, name)
{
	m_game = new GGZapGame();

	m_menu = new QPopupMenu(this);

	setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));

	contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 1);
	contextMenu()->insertSeparator();
	contextMenu()->insertItem(i18n("Configure"), menuconfigure, 3);
	contextMenu()->insertItem(i18n("About..."), menuabout, 4);

	connect(contextMenu(), SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu, SIGNAL(activated(int)), SLOT(slotLaunch(int)));

	m_state = stateidle;

	QToolTip::add(this, i18n("GGZap - Online Gaming Tool"));

	show();
}

GGZapTray::~GGZapTray()
{
	delete m_menu;
	delete m_game;
}

void GGZapTray::contextMenuAboutToShow(KPopupMenu *menu)
{
	QIconSet iconset = KGlobal::iconLoader()->loadIcon("1uparrow", KIcon::Small);

	m_menu->clear();

	m_game->clear();
	m_game->autoscan();
	for(int i = 0; i < m_game->count(); i++)
		m_menu->insertItem(iconset, QString("%1 (%2)").arg(m_game->name(i)).arg(m_game->frontend(i)), i);
}

void GGZapTray::slotLaunch(int gameid)
{
	emit signalLaunch(m_game->name(gameid), m_game->frontend(gameid));
}

void GGZapTray::slotMenu(int id)
{
	GGZapConfig configuration(this, "GGZapConfig");

	switch(id)
	{
		case menulaunch:
			if(m_state == stateidle)
			{
				contextMenu()->removeItem(menulaunch);
				contextMenu()->insertItem(i18n("Cancel game"), menucancel, 1);
			}
			m_state = stateactive;
			setMovie(QMovie(KGGZ_DIRECTORY "/ggzap/trayradar.mng"));
			break;
		case menucancel:
			emit signalCancel();
			setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));
			if(m_state == stateactive)
			{
				contextMenu()->removeItem(menucancel);
				contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 1);
			}
			m_state = stateidle;
			break;
		case menuconfigure:
			configuration.exec();
			break;
		case menuabout:
			const KAboutData *aboutdata = kapp->aboutData();
			KAboutApplication aboutdialog(aboutdata, this);
			aboutdialog.exec();
			break;
	}
}

