#include "embeddedcoreclient.h"

#include "playerlist.h"
#include "tablelist.h"
#include "roomlist.h"
#include "tabledialog.h"
#include "kggzcorelayer.h"
#include "player.h"

#include <kggzcore/coreclient.h>

#include <QIcon>
#include <QAction>
#include <QApplication>

#include <klocale.h>
#include <kiconloader.h>

#include <kchat.h>

// FIXME: in addition to the chat widget, there's also the game chat which could be part of libkggzgames

EmbeddedCoreClient::EmbeddedCoreClient(KGGZCore::CoreClient *core, KGGZCoreLayer *layer, bool withrooms)
: QObject(),
	m_core(core), m_corelayer(layer), m_tablenum(-1)
{
	if(withrooms)
		m_rooms = new RoomList();
	else
		m_rooms = NULL;

	if(!m_corelayer)
	{
		m_corelayer = new KGGZCoreLayer(this);
		m_corelayer->setCore(m_core);
	}
	if(!m_core)
		m_core = m_corelayer->core();

	m_players = new PlayerList();
	m_tables = new TableList();

	m_chat = new KChat(NULL, false);
	m_chat->setAutoAddMessages(false);

	QPixmap icon_launch = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_launch = new QAction(QIcon(icon_launch), i18n("Launch a new game"), this);
//	m_action_launch->setEnabled(false);

	QPixmap icon_join = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_join = new QAction(QIcon(icon_join), i18n("Join a running new game"), this);
	m_action_join->setEnabled(false);

	QPixmap icon_spectate = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_spectate = new QAction(QIcon(icon_spectate), i18n("Spectate a running game"), this);
	m_action_spectate->setEnabled(false);

	connect(m_chat, SIGNAL(signalSendMessage(int, const QString)), SLOT(slotChatEntered(int, const QString&)));
	connect(m_tables, SIGNAL(signalSelected(const KGGZCore::Table&, int)), SLOT(slotTable(const KGGZCore::Table, int)));

	connect(m_action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
	connect(m_action_join, SIGNAL(triggered(bool)), SLOT(slotJoin()));
	connect(m_action_spectate, SIGNAL(triggered(bool)), SLOT(slotSpectate()));

	setCore(m_core);
}

void EmbeddedCoreClient::setCore(KGGZCore::CoreClient *core)
{
	if(!core)
		return;

	m_core = core;
	m_corelayer->setCore(core);

	//QObject::disconnect(m_core->room(), 0, this, 0);

	connect(m_core->room(),
		SIGNAL(signalFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)),
		SLOT(slotFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)));
	connect(m_core->room(),
		SIGNAL(signalAnswer(KGGZCore::Room::AnswerMessage)),
		SLOT(slotAnswer(KGGZCore::Room::AnswerMessage)));
	connect(m_core->room(),
		SIGNAL(signalEvent(KGGZCore::Room::EventMessage)),
		SLOT(slotEvent(KGGZCore::Room::EventMessage)));
	connect(m_core->room(),
		SIGNAL(signalChat(QString, QString, KGGZCore::Room::ChatType)),
		SLOT(slotChat(QString, QString, KGGZCore::Room::ChatType)));
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

void EmbeddedCoreClient::slotLaunch()
{
	TableDialog tabledlg(/*this*/NULL);
	// FIXME: see ConnectionDialog.cpp for asynchronous+modal variant
	KGGZCore::GameType gametype = m_core->room()->gametype();
	tabledlg.setGameType(gametype);
	tabledlg.setIdentity(m_core->username());
	if(tabledlg.exec() == QDialog::Accepted)
	{
		m_corelayer->configureTable(tabledlg.table().description(), tabledlg.table().players());
		m_corelayer->launch();
	}
}

void EmbeddedCoreClient::slotJoin()
{
	m_corelayer->join(m_tablenum, false);
}

void EmbeddedCoreClient::slotSpectate()
{
	m_corelayer->join(m_tablenum, true);
}

void EmbeddedCoreClient::handleRoomlist()
{
	// populate room list here...
}

void EmbeddedCoreClient::handleTablelist()
{
	m_tables->clear();
	// FIXME: shouldn't be necessary to enforce the disabled join/spectate buttons
	//slotTable(KGGZCore::Table(), -1);
	QList<KGGZCore::Table> tables = m_core->room()->tables();
	for(int i = 0; i < tables.count(); i++)
	{
		KGGZCore::Table table = tables.at(i);
		m_tables->addConfiguration(table);
	}
}

void EmbeddedCoreClient::slotTable(const KGGZCore::Table& table, int pos)
{
	Q_UNUSED(table);

	m_tablenum = pos;

	m_action_join->setEnabled((pos != -1));
	m_action_spectate->setEnabled((pos != -1));
}

void EmbeddedCoreClient::slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	// FIXME!
	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::Room::tablelaunched:
			//activity(i18n("A table has been launched"));
			break;
		case KGGZCore::Room::tablejoined:
			break;
		case KGGZCore::Room::tableleft:
			break;
	}
}

void EmbeddedCoreClient::slotAnswer(KGGZCore::Room::AnswerMessage message)
{
	QList<KGGZCore::Player> players;
	QList<KGGZCore::Table> tables;

	switch(message)
	{
		case KGGZCore::Room::playerlist:
			m_players->clear();
			players = m_core->room()->players();
			for(int i = 0; i < players.count(); i++)
			{
				KGGZCore::Player player = players.at(i);
				Player *p = new Player(player.name());
				m_players->addPlayer(p);
			}
			break;
		case KGGZCore::Room::tablelist:
			handleTablelist();
			break;
	}
}

void EmbeddedCoreClient::slotEvent(KGGZCore::Room::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::chat:
			// FIXME: this is now in slotChat()
			break;
		case KGGZCore::Room::enter:
			// FIXME: appname != Vencedor?
			m_chat->addSystemMessage(i18n("Vencedor"), i18n("--> %1 has entered the room.").arg("SOMEBODY"));
			break;
		case KGGZCore::Room::leave:
			// FIXME: appname != Vencedor?
			m_chat->addSystemMessage(i18n("Vencedor"), i18n("<-- %1 has left the room.").arg("SOMEBODY"));
			break;
		case KGGZCore::Room::tableupdate:
			handleTablelist();
			break;
		case KGGZCore::Room::playerlag:
			break;
		case KGGZCore::Room::stats:
			break;
		case KGGZCore::Room::count:
			break;
		case KGGZCore::Room::perms:
			break;
		case KGGZCore::Room::libraryerror:
			break;
	}
}

void EmbeddedCoreClient::slotChat(QString sender, QString message, KGGZCore::Room::ChatType type)
{
	if(type == KGGZCore::Room::chatprivate)
	{
		message.prepend("[PRIVATE] ");
	}
	else if(type == KGGZCore::Room::chatbeep)
	{
		QApplication::beep();
		message = "[BEEP]";
	}

	// FIXME: colours and style?
	QFont namefont = m_chat->nameFont();
	QFont smfont = m_chat->systemMessageFont();
	namefont.setItalic(true);
	smfont.setBold(true);
	m_chat->setNameFont(namefont);
	m_chat->setSystemMessageFont(smfont);

	if(type != KGGZCore::Room::chatnormal)
		m_chat->addSystemMessage(sender, message);
	else
		m_chat->addMessage(sender, message);
}

void EmbeddedCoreClient::slotChatEntered(int id, const QString& msg)
{
	Q_UNUSED(id);

	// FIXME TODO: chatannounce, chattable

	KGGZCore::Room::ChatType type = KGGZCore::Room::chatnormal;
	QString receiver;
	QString message = msg;

	if((msg.length() > 0) && (msg.startsWith("/")))
	{
		QStringList list = message.split(" ");
		QString cmd = list.at(0);
		if(cmd == "/beep")
		{
			if(list.size() != 2)
			{
				return;
			}
			message = QString();
			type = KGGZCore::Room::chatbeep;
			receiver = list.at(1);
		}
		else if(cmd == "/msg")
		{
			if(list.size() < 3)
			{
				return;
			}
			message = msg.section(" ", 2);
			type = KGGZCore::Room::chatprivate;
			receiver = list.at(1);
		}
	}

	m_core->room()->sendchat(message, receiver, type);
}
