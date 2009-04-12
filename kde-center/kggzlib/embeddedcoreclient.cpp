#include "embeddedcoreclient.h"

#include "playerlist.h"
#include "tablelist.h"

#include <QLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>

#include <klocale.h>
#include <kiconloader.h>

#include <kchat.h>

// FIXME: in addition to the chat widget, there's also the game chat which could be part of libkggzgames

EmbeddedCoreClient::EmbeddedCoreClient()
: QMainWindow(),
	m_tablenum(-1)
{
	// FIXME: the layout is nearly identical to the one in Vencedor, with the exception of the room list

	QWidget *centralwidget = new QWidget();
	setCentralWidget(centralwidget);

//	m_rooms = new RoomList();
	m_players = new PlayerList();
	m_tables = new TableList();

	m_chat = new KChat(centralwidget, false);
	m_chat->setAutoAddMessages(false);

	QVBoxLayout *vbox = new QVBoxLayout();
//	vbox->addWidget(m_rooms);
	vbox->addWidget(m_players);
	vbox->addWidget(m_tables);

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addWidget(m_chat);

	centralwidget->setLayout(hbox);

	// FIXME: the toolbar is also partially identical (launch/join/spectate actions, maybe MOTD)
	// except for enabled launch button

	QToolBar *toolbar = new QToolBar();
	addToolBar(Qt::TopToolBarArea, toolbar);

	QPixmap icon_launch = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_launch = new QAction(QIcon(icon_launch), i18n("Launch a new game"), this);
	connect(m_action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
//	m_action_launch->setEnabled(false);
	toolbar->addAction(m_action_launch);

	QPixmap icon_join = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_join = new QAction(QIcon(icon_join), i18n("Join a running new game"), this);
	connect(m_action_join, SIGNAL(triggered(bool)), SLOT(slotJoin()));
	m_action_join->setEnabled(false);
	toolbar->addAction(m_action_join);

	QPixmap icon_spectate = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_spectate = new QAction(QIcon(icon_spectate), i18n("Spectate a running game"), this);
	connect(m_action_spectate, SIGNAL(triggered(bool)), SLOT(slotSpectate()));
	m_action_spectate->setEnabled(false);
	toolbar->addAction(m_action_spectate);

	connect(m_tables, SIGNAL(signalSelected(const KGGZCore::Table&, int)), SLOT(slotTable(const KGGZCore::Table, int)));

	setWindowTitle(i18n("GGZ Control Panel"));
	resize(600, 700);
	show();
}

void EmbeddedCoreClient::slotChat(QString sender, QString message, KGGZCore::Room::ChatType type)
{
}

// FIXME: core layer must be shared?! etc.
void EmbeddedCoreClient::slotLaunch()
{
//	TableDialog tabledlg(this);
//	// FIXME: see ConnectionDialog.cpp
//	KGGZCore::GameType gametype = m_core->room()->gametype();
//	tabledlg.setGameType(gametype);
//	tabledlg.setIdentity(m_core->username());
//	if(tabledlg.exec() == QDialog::Accepted)
//	{
//		KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
//		corelayer->setCore(m_core);
//		corelayer->configureTable(tabledlg.table().players());
//		corelayer->launch();
//	}
}

void EmbeddedCoreClient::slotJoin()
{
//	KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
//	corelayer->setCore(m_core);
//	corelayer->join(m_tablenum, false);
}

void EmbeddedCoreClient::slotSpectate()
{
//	KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
//	corelayer->setCore(m_core);
//	corelayer->join(m_tablenum, true);
}

void EmbeddedCoreClient::handleRoomlist()
{
	// populate table list instead of rooms here...
}

void EmbeddedCoreClient::slotTable(const KGGZCore::Table& table, int pos)
{
	Q_UNUSED(table);

	m_tablenum = pos;

	m_action_join->setEnabled((pos != -1));
	m_action_spectate->setEnabled((pos != -1));
}
