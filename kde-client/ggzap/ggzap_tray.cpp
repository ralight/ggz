#include "ggzap_tray.h"
#include "ggzap_game.h"
#include "ggzap_config.h"

#include <klocale.h>
#include <kpopupmenu.h>

#include <qpixmap.h>
#include <qmovie.h>

#include "config.h"
#include <iostream>

#include <stdio.h> /*tmp*/

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
	//slotMenu(menulaunch);
}

void GGZapTray::slotMenu(int id)
{
	switch(id)
	{
		case menulaunch:
printf("==LAUNCH\n");
			//m_launched = 1;
			contextMenu()->removeItem(menulaunch);
			contextMenu()->insertItem(i18n("Cancel game"), menucancel, 3);
			setMovie(QMovie(KGGZ_DIRECTORY "/ggzap/trayradar.mng"));
			break;
		case menucancel:
printf("CANCEL\n");
			//if(!m_launched) return;
			emit signalCancel();
			contextMenu()->removeItem(menucancel);
			contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 3);
			setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));
			break;
		case menuconfigure:
			if(!m_config) m_config = new GGZapConfig(NULL, "GGZapConfig");
			m_config->show();
			break;
	}
}

