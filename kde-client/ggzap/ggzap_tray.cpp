#include "ggzap_tray.h"
#include "ggzap_game.h"

#include <klocale.h>
#include <kpopupmenu.h>

#include <qpixmap.h>

#include "config.h"
#include <iostream>

GGZapTray::GGZapTray(QWidget *parent, const char *name)
: KSystemTray(parent, name)
{
	m_game = new GGZapGame();

	m_menu = new QPopupMenu(this);

	setPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/tray.png"));

	contextMenu()->insertItem(i18n("Configure"), menuconfigure, 1);
	contextMenu()->insertSeparator();
	contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 3);

	connect(contextMenu(), SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(m_menu, SIGNAL(activated(int)), SLOT(slotLaunch(int)));

	show();
}

GGZapTray::~GGZapTray()
{
	delete m_menu;
	delete m_game;
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
	slotMenu(menulaunch);
}

void GGZapTray::slotMenu(int id)
{
	switch(id)
	{
		case menulaunch:
			contextMenu()->removeItem(menulaunch);
			contextMenu()->insertItem(i18n("Cancel game"), menucancel, 3);
			break;
		case menucancel:
			emit signalCancel();
			contextMenu()->removeItem(menucancel);
			contextMenu()->insertItem(i18n("Launch a game"), m_menu, menulaunch, 3);
			break;
		case menuconfigure:
			break;
	}
}

