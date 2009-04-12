#include "embeddedcoreclient.h"

#include "playerlist.h"
#include "tablelist.h"
#include "roomlist.h"
#include "tabledialog.h"
#include "kggzcorelayer.h"

#include <kggzcore/coreclient.h>

#include <QIcon>
#include <QAction>

#include <klocale.h>
#include <kiconloader.h>

#include <kchat.h>

// FIXME: in addition to the chat widget, there's also the game chat which could be part of libkggzgames

EmbeddedCoreClient::EmbeddedCoreClient(KGGZCore::CoreClient *core, bool withrooms)
: QObject(),
	m_core(core), m_tablenum(-1)
{
	if(withrooms)
		m_rooms = new RoomList();
	else
		m_rooms = NULL;

	m_players = new PlayerList();
	m_tables = new TableList();

	m_chat = new KChat(NULL, false);
	m_chat->setAutoAddMessages(false);

	QPixmap icon_launch = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_launch = new QAction(QIcon(icon_launch), i18n("Launch a new game"), this);
	connect(m_action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
//	m_action_launch->setEnabled(false);

	QPixmap icon_join = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_join = new QAction(QIcon(icon_join), i18n("Join a running new game"), this);
	connect(m_action_join, SIGNAL(triggered(bool)), SLOT(slotJoin()));
	m_action_join->setEnabled(false);

	QPixmap icon_spectate = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_spectate = new QAction(QIcon(icon_spectate), i18n("Spectate a running game"), this);
	connect(m_action_spectate, SIGNAL(triggered(bool)), SLOT(slotSpectate()));
	m_action_spectate->setEnabled(false);

	connect(m_tables, SIGNAL(signalSelected(const KGGZCore::Table&, int)), SLOT(slotTable(const KGGZCore::Table, int)));

	connect(m_action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
	connect(m_action_join, SIGNAL(triggered(bool)), SLOT(slotJoin()));
	connect(m_action_spectate, SIGNAL(triggered(bool)), SLOT(slotSpectate()));
}

KChat *EmbeddedCoreClient::widget_chat()
{
	return m_chat;
}

PlayerList *EmbeddedCoreClient::widget_players()
{
	return m_players;
}

TableList *EmbeddedCoreClient::widget_tables()
{
	return m_tables;
}

RoomList *EmbeddedCoreClient::widget_rooms()
{
	return m_rooms;
}

QAction *EmbeddedCoreClient::action_launch()
{
	return m_action_launch;
}

QAction *EmbeddedCoreClient::action_join()
{
	return m_action_join;
}

QAction *EmbeddedCoreClient::action_spectate()
{
	return m_action_spectate;
}

void EmbeddedCoreClient::slotChat(QString sender, QString message, KGGZCore::Room::ChatType type)
{
}

// FIXME: core layer must be shared?! etc.
void EmbeddedCoreClient::slotLaunch()
{
	TableDialog tabledlg(/*this*/NULL);
	// FIXME: see ConnectionDialog.cpp for asynchronous+modal variant
	KGGZCore::GameType gametype = m_core->room()->gametype();
	tabledlg.setGameType(gametype);
	tabledlg.setIdentity(m_core->username());
	if(tabledlg.exec() == QDialog::Accepted)
	{
		KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
		corelayer->setCore(m_core);
		corelayer->configureTable(tabledlg.table().description(), tabledlg.table().players());
		corelayer->launch();
	}
}

void EmbeddedCoreClient::slotJoin()
{
	KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
	corelayer->setCore(m_core);
	corelayer->join(m_tablenum, false);
}

void EmbeddedCoreClient::slotSpectate()
{
	KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
	corelayer->setCore(m_core);
	corelayer->join(m_tablenum, true);
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
