//////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZ: The main GGZ Gaming Zone object. Controls servers, rooms and games/rooms. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZ.h"

// KGGZ includes
#include "KGGZWorkspace.h"
#include "KGGZSplash.h"
#ifdef KGGZ_BROWSER
#include "KGGZBrowser.h"
#endif
#include "KGGZGrubby.h"
#include "KGGZPrefEnv.h"
#include "KGGZSelector.h"
#include "KGGZGameInfoDialog.h"
#include "KGGZTeam.h"

// KDE includes
#include <kmessagebox.h>
#include <klocale.h>
#include <kurl.h>
#include <krandomsequence.h>
#ifdef KGGZ_WALLET
#include <klineeditdlg.h>
#include <kwallet.h>
#endif
#include <ksimpleconfig.h>
#include <kapplication.h>

// Qt includes
#include <qiconview.h>
#include <qlayout.h>
#include <qdir.h>

// System includes
#include <stdio.h>
#include <iostream>
#include <cstdlib>

// GGZCore++ includes
#include "GGZCoreConfio.h"
#include "GGZCorePlayer.h"

KGGZ::KGGZ(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	int result;
	QLayout *vbox;

	m_channelfd = -1;
	m_gamefd = -1;
	kggzroom = NULL;
	kggzserver = NULL;
	m_launch = NULL;
	kggzgame = NULL;
#ifdef KGGZ_BROWSER
	m_browser = NULL;
#endif
	m_motd = NULL;
	m_grubby = NULL;
	m_prefenv = NULL;
	m_selector = NULL;
	m_module = NULL;
	m_table = NULL;
	m_gameinfo = NULL;
	m_connect = NULL;
	m_killserver = 0;
	m_dns = NULL;
	m_gameinfodialog = NULL;
	m_team = NULL;
	m_sn_game = NULL;
	m_sn_server = NULL;

	setEraseColor(QColor(0, 0, 0));

	m_splash = new KGGZSplash(this, "splash");

#ifdef KGGZ_BROWSER
	m_browser = new KGGZBrowser(this, "browser");
#endif

	m_workspace = new KGGZWorkspace(this, "workspace");

	connect(m_workspace->widgetChat(), SIGNAL(signalChat(QString, QString, int)), SLOT(slotChat(QString, QString, int)));
	connect(m_workspace->widgetChat(), SIGNAL(signalAdmin(int, QString, QString)), SLOT(slotAdmin(int, QString, QString)));
	connect(m_workspace->widgetUsers(), SIGNAL(signalChat(QString, QString, int)), SLOT(slotChat(QString, QString, int)));
	connect(m_workspace->widgetLogo(), SIGNAL(signalInfo()), SLOT(menuGameInfo()));

	m_core = new GGZCore();
	result = m_core->init(GGZCore::parser | GGZCore::modules); 
	if(result == -1)
	{
		KGGZDEBUG("Critical: Could not initialize ggzcore!\n");
		m_core = NULL;
	}

	m_config = NULL;
	m_connfront = false;
	
	readConfiguration(true);
	
	kggzroomcallback = new KGGZCallback(this, COLLECTOR_ROOM);
	kggzservercallback = new KGGZCallback(this, COLLECTOR_SERVER);
	kggzgamecallback = new KGGZCallback(this, COLLECTOR_GAME);

	vbox = new QVBoxLayout(this);
	vbox->add(m_splash);
#ifdef KGGZ_BROWSER
	vbox->add(m_browser);
#endif
	vbox->add(m_workspace);

	startTimer(100);
}

KGGZ::~KGGZ()
{
	dispatcher();

	exit(0);
}

void KGGZ::readConfiguration(bool immediate)
{
	int value, result;

	if(m_config) delete m_config;
	m_config = new GGZCoreConf();

	result = m_config->init(KGGZ_DIRECTORY "/kggzrc",
			(QDir::homeDirPath() + "/.ggz/kggz.rc").utf8());
	if(result == -1)
	{
		KGGZDEBUG("Critical: Could not open configuration file!\n");
		m_config = NULL;
		m_showmotd = 1;
	}
	else
	{
		// Read out configuration and setup things
		if(immediate)
		{
			if(m_config->read("Preferences", "Showdialog", 1))
				menuConnect();
		}
		value = m_config->read("Preferences", "Chatlog", 0);
		m_workspace->widgetChat()->setLogging(value);
		value = m_config->read("Preferences", "Speech", 0);
		m_workspace->widgetChat()->setSpeech(value);
		value = m_config->read("Preferences", "Timestamps", 0);
		m_workspace->widgetChat()->setTimestamp(value);

		m_showmotd = m_config->read("Preferences", "MOTD", 1);
	}
}

void KGGZ::autoconnect(QString uri)
{
	QString user, password;
	int mode;
	KRandomSequence seq;
	KURL url(uri);

	if(url.protocol() != "ggz") return;

	if(m_connect) m_connect->hide();

	if(url.hasUser()) user = url.user();
	else user = "guest-" + QString("%1").arg(seq.getLong(10000));
	if(url.hasPass())
	{
		password = url.pass();
		mode = GGZCoreServer::normal;
	}
	else
	{
		password = "";
		mode = GGZCoreServer::guest;
	}

	slotConnected(url.host(), (url.port() ? url.port() : 5688), user, password, mode);
}

void KGGZ::resizeEvent(QResizeEvent *e)
{
	m_workspace->resize(e->size());
}

void KGGZ::slotConnected(QString host, int port, QString username, QString password, int mode)
{
#ifdef KGGZ_WALLET
	QString p, entry;
	KWallet::Wallet *w = NULL;

	if(mode == GGZCoreServer::normal)
	{
		w = KWallet::Wallet::openWallet("kggz");
		if(w)
		{
			entry = QString("%1@%2").arg(username).arg(host);
			w->setFolder("passwords");
			w->readPassword(entry, p);

			/*KMessageBox::information(this,
				i18n("Wallet password: %1").arg(password), i18n("debug"));*/
		}
		else KMessageBox::error(this,
			i18n("The wallet could not be opened."), i18n("Connection"));

		if(p.isEmpty())
		{
			p = KLineEditDlg::getText(i18n("Password"),
				i18n("Password not found, please input:"), NULL, NULL);
			if((p) && (w))
				w->writePassword(entry, p);
		}
		password = p;

	}
#endif

	if(m_connect->optionServer())
	{
		KGGZDEBUG("Start server\n");
		host = "localhost";
		menuServerLaunch();
	}

	if(m_connect->optionSecure())
	{
		KMessageBox::information(this,
			i18n("Although the TLS implementation may work, it is not widely tested yet and in no way secure!"),
			i18n("Security warning"));
	}

	KGGZDEBUG("Connect with: host=%s port=%i username=%s password=%s mode=%i encryption=%i\n",
		host.utf8().data(), port, username.utf8().data(),
		password.utf8().data(), mode, m_connect->optionSecure());

	if(m_dns) delete m_dns;
	m_dns = new QDns(host, QDns::A);
	connect(m_dns, SIGNAL(resultsReady()), SLOT(slotConnectedStart()));

	m_save_loginmode = mode;
	m_save_username = username;
	m_save_password = password;
	m_save_host = host;
	m_save_hostname = host;
	m_save_port = port;
	m_save_encryption = m_connect->optionSecure();
}

void KGGZ::slotConnectedStart()
{
	int result;
	QValueList<QHostAddress> list;
	QHostAddress addr;

	if(m_dns->addresses().isEmpty())
	{
		KMessageBox::error(this, i18n("Host not found!"), i18n("Error!"));
		menuConnect();
		return;
	}
	list = m_dns->addresses();
	addr = (*list.at(0));
	m_save_host = addr.toString();
	KGGZDEBUG("Host resolved to: %s\n", m_save_host.utf8().data());

	delete m_dns;
	m_dns = NULL;

	kggzserver = new GGZCoreServer();
	attachServerCallbacks();
	kggzserver->logSession(QString("%1/.ggz/kggz.xml-log").arg(QDir::homeDirPath()).utf8());

	kggzserver->setHost(m_save_host, m_save_port, m_save_encryption);
	result = kggzserver->connect();
	/*if(result == -1)
	{
		if(kggzserver)
		{
			detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			KMessageBox::error(this, i18n("Attempt to connect refused!"), i18n("Error!"));
			menuConnect();
		}
		return;
	}*/
}

void KGGZ::menuDisconnect()
{
	eventLeaveTable(0);
    eventLeaveRoom();

	if(!kggzserver)
	{
		KGGZDEBUG("Critical: No server found!\n");
		return;
	}
	if((!kggzserver->isLoggedIn()) || (!kggzserver->isOnline()))
	{
		KGGZDEBUG("Critical: Not logged in or connected!\n");
		return;
	}

	KGGZDEBUG("=> logout now! <=\n");
	kggzserver->logout();
	//m_killserver = 1;
}

void KGGZ::menuServerLaunch()
{
	int result;
	GGZCoreConfio *config;
	char *process;

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()).utf8(),
		GGZCoreConfio::readwrite | GGZCoreConfio::create);
	process = config->read("Environment", "Server", "/usr/bin/ggzd");
	delete config;

	if((!process) && (!strlen(process)))
	{
		KMessageBox::information(this,
			i18n("GGZ Gaming Zone server not found.\nPlease configure it in the settings dialog.\n"),
			i18n("Note"));
		return;
	}

	result = KGGZCommon::launchProcess("ggzd", process);
	GGZCoreConfio::free(process);

	switch(result)
	{
		case -1:
			KMessageBox::error(this, i18n("Could not start the server!"), i18n("Error!"));
			break;
		case -2:
			KMessageBox::error(this, i18n("The GGZ server is already running!"), i18n("Error!"));
			break;
		default:
			emit signalMenu(MENUSIG_SERVERSTART);
			break;
	}
}

void KGGZ::menuServerKill()
{
	int result;

	result = KGGZCommon::killProcess("ggzd");
	switch(result)
	{
		case -3:
			KMessageBox::error(this, i18n("The server isn't running anymore."), i18n("Error!"));
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
		case -2:
			KMessageBox::error(this, i18n("You don't have the privileges to stop the server."), i18n("Error!"));
			break;
		case -1:
			KMessageBox::error(this, i18n("An unknown error occurred."), i18n("Error!"));
			break;
		case 0:
			KMessageBox::error(this, i18n("Could only kill server via sigkill!"), i18n("Error!"));
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
		default:
			emit signalMenu(MENUSIG_SERVERSTOP);
			break;
	}
}

void KGGZ::menuConnect()
{
	if(!m_connect)
	{
		m_connect = new KGGZConnect(this, "connect");
		connect(m_connect, SIGNAL(signalConnect(QString, int, QString, QString, int)),
			SLOT(slotConnected(QString, int, QString, QString, int)));
	}
	m_connect->show();
	m_connfront = true;
}

void KGGZ::menuMotd()
{
	if(m_motd) m_motd->show();
	else KMessageBox::sorry(this, i18n("No MOTD found for this server."), i18n("No MOTD found"));
}

void KGGZ::dispatcher()
{
	if(m_connect) delete m_connect;
	if(kggzroom) delete kggzroom;
	if(kggzserver) delete kggzserver;
	if(kggzgamecallback) delete kggzgamecallback;
	if(kggzroomcallback) delete kggzroomcallback;
	if(kggzservercallback) delete kggzservercallback;
	if(m_table) delete m_table;
	if(m_module) delete m_module;
	if(m_config) delete m_config;
	if(m_core) delete m_core;
}

void KGGZ::timerEvent(QTimerEvent *e)
{
	Q_UNUSED(e);

	if(m_connect)
		if(m_connfront)
		{
			m_connect->raise();
			m_connfront = false;
		}

	if(m_channelfd >= 0)
	{
		// FIXME: ggzcore++
		ggzcore_server_read_data(kggzserver->server(), m_channelfd);
	}

	if(m_killserver)
	{
		eventLeaveTable(0);
		eventLeaveRoom();
		if(kggzserver)
		{
			kggzserver->disconnect();
			detachServerCallbacks();
			KGGZDEBUG("delete kggzserver;\n");
			delete kggzserver;
			kggzserver = NULL;
			if(m_sn_server)
			{
				delete m_sn_server;
				m_sn_server = NULL;
			}
		}
		emit signalMenu(MENUSIG_DISCONNECT);
		m_killserver = 0;
	}
	if(!kggzserver) return;
}

void KGGZ::slotServerData()
{
	//KGGZDEBUG("slotServerData\n");
	if(kggzserver) kggzserver->dataRead();
}

void KGGZ::slotGameData()
{
	if(kggzgame) kggzgame->dataRead();
}

void KGGZ::listTables()
{
	int number;
	GGZCoreTable *table;
	GGZCoreGametype *gametype;
	int tableseats, tableseatsopen;
	QString playername, tabledescription;
	QString name;

	if(!kggzroom)
	{
		KGGZDEBUG("Critical! We are not in a room, are we?\n");
		return;
	}
	if(!m_workspace)
	{
		KGGZDEBUG("Feurio! No workspace here!\n");
		return;
	}
	gametype = kggzroom->gametype();
	if(!gametype)
	{
		KGGZDEBUG("Critical: Error when fetching game type!\n");
		return;
	}
	m_workspace->widgetTables()->reset();
	number = kggzroom->countTables();
	for(int i = 0; i < number; i++)
	{
		m_workspace->widgetUsers()->addTable(i);
		table = kggzroom->table(i);
		tabledescription = table->description();
		tableseats = table->countSeats();
		tableseatsopen = 0;
		for(int j = 0; j < tableseats; j++)
		{
			KGGZDEBUG(" ** table %i, seat %i: player type is %i (state: %i)\n", i, j, table->playerType(j), table->state());

			switch(table->playerType(j))
			{
				case GGZ_SEAT_OPEN:
					KGGZDEBUG("GGZ_SEAT_OPEN (%i)\n", GGZ_SEAT_OPEN);
					break;
				case GGZ_SEAT_BOT:
					KGGZDEBUG("GGZ_SEAT_BOT (%i)\n", GGZ_SEAT_BOT);
					break;
				case GGZ_SEAT_NONE:
					KGGZDEBUG("GGZ_SEAT_NONE (%i)\n", GGZ_SEAT_NONE);
					break;
				case GGZ_SEAT_RESERVED:
					KGGZDEBUG("GGZ_SEAT_RESERVED (%i)\n", GGZ_SEAT_RESERVED);
					break;
				case GGZ_SEAT_PLAYER:
					KGGZDEBUG("GGZ_SEAT_PLAYER (%i)\n", GGZ_SEAT_PLAYER);
					break;
				default:
					KGGZDEBUG("UNKNOWN (%i)\n", table->playerType(i));
			}

			if(table->playerType(j) == GGZ_SEAT_OPEN
				|| table->playerType(j) == GGZ_SEAT_RESERVED) tableseatsopen++;
			else
			{
				if(table->playerType(j) == GGZ_SEAT_PLAYER)
				{
					playername = table->playerName(j);
					if(playername.isEmpty())
					{
						playername = i18n("(unknown)");
						tableseatsopen++;
					}
					m_workspace->widgetUsers()->addTablePlayer(i, playername);
				}
			}
		}
		m_workspace->widgetTables()->add(gametype->name(),
			QString::fromUtf8(tabledescription), tableseats - tableseatsopen, tableseats);
	}
}

void KGGZ::listPlayers()
{
	int number;
	GGZCorePlayer *player;
	char *playername;
	int type;

	if(!kggzroom) return;
	m_workspace->widgetUsers()->removeall();
	m_workspace->widgetChat()->chatline()->removeAll();
	if(m_grubby) m_grubby->removeAll();
	number = kggzroom->countPlayers();
	for(int i = 0; i < number; i++)
	{
		player = kggzroom->player(i);
		playername = player->name();
		m_workspace->widgetUsers()->add(playername);
		switch(player->type())
		{
			case GGZCorePlayer::guest:
				type = KGGZUsers::assignguest;
				break;
			case GGZCorePlayer::normal:
				type = KGGZUsers::assignplayer;
				break;
			case GGZCorePlayer::admin:
				type = KGGZUsers::assignadmin;
				break;
			case GGZCorePlayer::host:
				type = KGGZUsers::assignhost;
				break;
			case GGZCorePlayer::bot:
				type = KGGZUsers::assignbot;
				break;
			case GGZCorePlayer::unknown:
			default:
				type = KGGZUsers::assignunknown;
				break;
		}
		m_workspace->widgetUsers()->assignRole(playername, type);
		m_workspace->widgetChat()->chatline()->addPlayer(playername);
		if((m_grubby) && (type == KGGZUsers::assignbot)) m_grubby->addPlayer(playername);
	}
	m_workspace->widgetUsers()->assignSelf(m_save_username);
}

void KGGZ::lagPlayers()
{
	int number, lag;
	GGZCorePlayer *player;
	char *playername;

	if(!kggzroom) return;
	number = kggzroom->countPlayers();
	for(int i = 0; i < number; i++)
	{
		player = kggzroom->player(i);
		playername = player->name();
		lag = player->lag();
		m_workspace->widgetUsers()->setLag(playername, lag);
	}
}

void KGGZ::gameCollector(unsigned int id, const void* data)
{
	int result;
	int seats;
	QString description;
	GGZCoreGametype *gametype;

	Q_UNUSED(data);

	switch(id)
	{
		case GGZCoreGame::launched:
			KGGZDEBUG("launched\n");
			m_workspace->widgetChat()->receive(NULL, i18n("Launched game"), KGGZChat::RECEIVE_ADMIN);
			m_gamefd = kggzgame->fd();
			m_sn_game = new QSocketNotifier(m_gamefd, QSocketNotifier::Read, this);
			connect(m_sn_game, SIGNAL(activated(int)), SLOT(slotGameData()));
			break;
		case GGZCoreGame::launchfail:
			KGGZDEBUG("launchfail\n");
			break;
		case GGZCoreGame::negotiated:
			KGGZDEBUG("negotiated\n");
			kggzserver->createChannel();
			break;
		case GGZCoreGame::negotiatefail:
			KGGZDEBUG("negotiatefail\n");
			break;
		case GGZCoreGame::playing:
			KGGZDEBUG("playing\n");
			if(m_gameinfo->type() == KGGZGameInfo::typelaunch)
			{
				gametype = kggzroom->gametype();
				if((!gametype) || (!gametype->gametype()) || (!m_gameinfo))
				{
					KGGZDEBUG("Erm... would you mind to give me the game type?\n");
					return;
				}

				if(!m_launch)
				{
					KGGZDEBUG("Huh?\n");
					return;
				}
				if(m_launch->description().isEmpty()) description = "";
				else description = m_launch->description();
				seats = m_launch->seats();

				m_table = new GGZCoreTable();
				m_table->init(gametype->gametype(), description.utf8(), seats);
				for(int i = 0; i < seats; i++)
				{
					switch(m_launch->seatType(i))
					{
						case KGGZLaunch::seatplayer:
							KGGZDEBUG("* %i: player\n", i);
							break;
						case KGGZLaunch::seatopen:
							KGGZDEBUG("* %i: open\n", i);
							break;
						case KGGZLaunch::seatbot:
							KGGZDEBUG("* %i: bot (%s)\n", i, m_launch->reservation(i).utf8().data());
							if(m_launch->reservation(i).isNull())
								m_table->addBot(NULL, i);
							else
								m_table->addBot(m_launch->reservation(i).utf8(), i);
							break;
						case KGGZLaunch::seatreserved:
							KGGZDEBUG("* %i: reserved (%s)\n", i, m_launch->reservation(i).utf8().data());
							m_table->addReserved(m_launch->reservation(i).utf8(), i);
							break;
						case KGGZLaunch::seatunused:
							KGGZDEBUG("* %i: unused.\n", i);
							break;
						case KGGZLaunch::seatunknown:
						default:
							KGGZDEBUG("* %i: unknown!\n", i);
					}
				}

				result = kggzroom->launchTable(m_table->table());
				delete m_launch;
				m_launch = NULL;
				if(result < 0)
				{
					delete m_module;
					m_module = NULL;
					delete m_table;
					m_table = NULL;
					KMessageBox::error(this, i18n("Failed launching the requested table!"), i18n("Error!"));
					return;
				}

				if(m_table)
				{
					delete m_table;
					m_table = NULL;
				}
			}
			else
			{
				if(m_gameinfo->type() == KGGZGameInfo::typejoin)
					result = kggzroom->joinTable(m_gameinfo->table());
				else
					result = kggzroom->joinTableSpectator(m_gameinfo->table());
				if(result < 0)
				{
					delete m_module;
					m_module = NULL;
					KMessageBox::error(this, i18n("Failed joining the requested table!"), i18n("Error!"));
					return;
				}
			}
			break;
		default:
			KGGZDEBUG("unknown!\n");
			break;
	}
}

void KGGZ::roomCollector(unsigned int id, const void* data)
{
	QString chatsender, chatmessage;
	int chattype = GGZCoreRoom::chatnormal;
	QString buffer;
	GGZRoomChangeEventData *event;
	QString toroom;
	QString fromroom;
	int roomnumber;
	QString player;
	GGZRoom *room;

	roomnumber = 0;

	if(id == GGZCoreRoom::chatevent)
		emit signalActivity(ACTIVITY_DIRECT);
	else if(id != GGZCoreRoom::playerlag)
		emit signalActivity(ACTIVITY_ROOM);

	switch(id)
	{
		case GGZCoreRoom::chatevent:
			chattype = ((GGZChatEventData*)data)->type;
			chatsender = ((GGZChatEventData*)data)->sender;
			chatmessage = ((GGZChatEventData*)data)->message;
			KGGZDEBUG("Chat receives: %s from %s\n",
				chatmessage.utf8().data(), chatsender.utf8().data());
			if((!m_workspace) || (!m_workspace->widgetChat()))
			{
				KGGZDEBUG("Critical: Workspace/Chat absent!\n");
				return;
			}
			break;
		case GGZCoreRoom::stats:
			player = QString((char*)data);
			break;
		case GGZCoreRoom::count:
			roomnumber = *(int*)data;
			break;
		case GGZCoreRoom::perms:
			player = QString((char*)data);
			break;
	}

	switch(id)
	{
		case GGZCoreRoom::playerlist:
			KGGZDEBUG("playerlist\n");
			listPlayers();
			break;
		case GGZCoreRoom::tablelist:
			KGGZDEBUG("tablelist\n");
			listTables();
			break;
		case GGZCoreRoom::chatevent:
			switch(chattype)
			{
				case GGZCoreRoom::chatnormal:
					KGGZDEBUG("chatnormal\n");
					if(chatmessage.startsWith("/me "))
					{
						m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_ME);
					}
					else
					{
						if(chatsender == m_save_username)
							m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_OWN);
						else
						{
							KGGZDEBUG("%s != %s\n", chatsender.utf8().data(),
								m_save_username.utf8().data());
							m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_CHAT);
						}
					}
					break;
				case GGZCoreRoom::chatannounce:
					KGGZDEBUG("chatannounce\n");
					m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_ANNOUNCE);
					break;
				case GGZCoreRoom::chatprivate:
					KGGZDEBUG("chatprivate\n");
					if((m_grubby) && (m_grubby->isVisible())) m_grubby->answer(chatmessage);
					else m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_PERSONAL);
					break;
				case GGZCoreRoom::chatbeep:
					KGGZDEBUG("chatbeep\n");
					m_workspace->widgetChat()->beep();
					m_workspace->widgetChat()->receive(NULL, i18n("You have been beeped by %1.").arg(chatsender), KGGZChat::RECEIVE_PERSONAL);
					break;
				case GGZCoreRoom::chattable:
					m_workspace->widgetChat()->receive(chatsender, chatmessage, KGGZChat::RECEIVE_TABLE);
					break;
			}
			break;
		case GGZCoreRoom::enter:
			KGGZDEBUG("enter\n");
			event = (GGZRoomChangeEventData*)data;
			/*if(event->from_room == -2) fromroom = i18n("(unknown origin)");
			else*/ if(event->from_room == NULL) fromroom = i18n("first room after login");
			else fromroom = QString("from room %1").arg(kggzserver->room(event->from_room)->name());
			buffer = i18n("%1 enters the room (%2).").arg(
				event->player_name).arg(
				fromroom);
			//buffer.append(event->player_name);
			//buffer.append(i18n(" enters the room."));
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetUsers()->add(event->player_name);
			m_workspace->widgetChat()->chatline()->addPlayer(event->player_name);
			//if(m_grubby) m_grubby->addPlayer(event->player_name);
			roomnumber = ggzcore_server_get_room_num(kggzserver->server(), event->to_room);
			kggzroom = new GGZCoreRoom(event->to_room); //kggzserver->room(roomnumber);
			emit signalRoomChanged(kggzroom->name(), kggzroom->gametype()->protocolEngine(), roomnumber, kggzroom->countPlayers());
			break;
		case GGZCoreRoom::leave:
			KGGZDEBUG("leave\n");
			event = (GGZRoomChangeEventData*)data;
			/*if(event->to_room == -2) toroom = i18n("(unknown destination)");
			else*/ if(event->to_room == NULL) toroom = i18n("disconnect from server");
			else toroom = QString("to room %1").arg(kggzserver->room(event->to_room)->name());
			buffer = i18n("%1 has left the room (%2).").arg(
				event->player_name).arg(
				toroom);
			//buffer.append(event->player_name);
			//buffer.append(i18n(" has left the room."));
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetUsers()->remove(event->player_name);
			KGGZDEBUG("remove from chatline\n");
			m_workspace->widgetChat()->chatline()->removePlayer(event->player_name);
			roomnumber = ggzcore_server_get_room_num(kggzserver->server(), event->from_room);
			kggzroom = new GGZCoreRoom(event->from_room); //kggzserver->room(roomnumber);
			emit signalRoomChanged(kggzroom->name(), kggzroom->gametype()->protocolEngine(), roomnumber, kggzroom->countPlayers());
			KGGZDEBUG("Leave room event: ready\n");
			break;
		case GGZCoreRoom::tableupdate:
			KGGZDEBUG("tableupdate\n");
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunched:
			KGGZDEBUG("tablelaunched\n");
			m_workspace->widgetChat()->receive(NULL, i18n("Launched table"), KGGZChat::RECEIVE_ADMIN);
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablelaunchfail:
			KGGZDEBUG("tablelaunchfail\n");
			KMessageBox::error(this, i18n("ERROR: Couldn't launch table!"), i18n("Error!"));
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tablejoined:
			KGGZDEBUG("tablejoined\n");
			m_workspace->widgetChat()->receive(NULL, i18n("Joined table"), KGGZChat::RECEIVE_ADMIN);
			slotGameFire();
			break;
		case GGZCoreRoom::tablejoinfail:
			KGGZDEBUG("tablejoinfail\n");
			KMessageBox::error(this, i18n("ERROR: Couldn't join table!"), i18n("Error"));
			listPlayers();
			listTables();
			break;
		case GGZCoreRoom::tableleft:
			KGGZDEBUG("tableleft\n");
			delete m_sn_game;
			m_sn_game = NULL;
			m_gamefd = -1;
			if(kggzgame)
			{
				// XXX: what about eventLeaveGame?
				detachGameCallbacks();
				delete kggzgame;
				kggzgame = NULL;
			}
			eventLeaveTable(0); /* was 1 in GGZCoreGame::over */
			m_workspace->widgetChat()->receive(NULL, i18n("Game over"), KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetChat()->receive(NULL, i18n("Left table"), KGGZChat::RECEIVE_ADMIN);
			emit signalMenu(MENUSIG_GAMEOVER);
			break;
		case GGZCoreRoom::tableleavefail:
			KGGZDEBUG("tableleavefail\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Couldn't leave table!"), KGGZChat::RECEIVE_ADMIN);
			break;
		/*case GGZCoreRoom::tabledata:
			KGGZDEBUG("tabledata\n");
			if(kggzgame) kggzgame->dataSend((char*)data);
			break;*/
		case GGZCoreRoom::playerlag:
			lagPlayers();
			break;
		case GGZCoreRoom::stats:
			m_workspace->widgetChat()->receive(NULL, i18n("Info: Statistics for %1 changed.").arg(player), KGGZChat::RECEIVE_ADMIN);
			break;
		case GGZCoreRoom::count:
			//GGZCoreRoom *tmproom = new GGZCoreRoom();
			KGGZDEBUG("count\n");
			room = ggzcore_server_get_nth_room(kggzserver->server(), roomnumber);
			emit signalRoomChanged(ggzcore_room_get_name(room),
				ggzcore_gametype_get_prot_engine(ggzcore_room_get_gametype(room)),
				roomnumber, ggzcore_room_get_num_players(room));
			break;
		case GGZCoreRoom::perms:
			KGGZDEBUG("perms!\n");
			KGGZDEBUG("Player: %s\n", player.utf8().data());
			break;
		default:
			KGGZDEBUG("unknown\n");
			break;
	}
}

void KGGZ::serverCollector(unsigned int id, const void* data)
{
	int result;
	QString buffer;
	GGZCoreGametype *gametype;
#ifdef KGGZ_WALLET
	KWallet::Wallet *w = NULL;
	QString entry;
#endif
	int roomnumber;
	GGZMotdEventData *motd;
	const GGZErrorEventData *error = (GGZErrorEventData*)data;

	emit signalActivity(ACTIVITY_ROOM);

	switch(id)
	{
		case GGZCoreServer::connected:
			KGGZDEBUG("connected\n");
			// possibly ggzcore bug:
			// state has not been updated yet here
//			while((kggzserver) && (!kggzserver->isOnline())) kggzserver->dataRead();
			if(!kggzserver) return;
			m_sn_server = new QSocketNotifier(kggzserver->fd(), QSocketNotifier::Read, this);
			connect(m_sn_server, SIGNAL(activated(int)), SLOT(slotServerData()));
			kggzserver->setLogin(m_save_loginmode,
				m_save_username.utf8(), m_save_password.utf8());
			break;
		case GGZCoreServer::connectfail:
			KGGZDEBUG("connectfail\n");
			KMessageBox::error(this, i18n("Couldn't connect to server!"), i18n("Error!"));
			m_killserver = 1;
			menuConnect();
			break;
		case GGZCoreServer::negotiated:
			KGGZDEBUG("negotiated\n");
			result = kggzserver->login();
			if(result == -1)
			{
				KMessageBox::error(this, i18n("Attempt to login refused!"), i18n("Error!"));
				m_killserver = 1;
				menuConnect();
				return;
			}
			break;
		case GGZCoreServer::negotiatefail:
			KGGZDEBUG("negotiatefail\n");

			/*kggzserver->disconnect();*/
			/*detachServerCallbacks();
			delete kggzserver;
			kggzserver = NULL;
			delete m_sn_server;
			m_sn_server = NULL;*/
			/*m_killserver = 1;*/

			KMessageBox::error(this, i18n("Could not connect, the server version may be incompatible."), i18n("Error!"));

			menuConnect();
			break;
		case GGZCoreServer::loggedin:
			KGGZDEBUG("loggedin\n");
			emit signalMenu(MENUSIG_LOGIN);
			buffer = i18n("logged in as %1@%2:%3").arg(m_save_username).arg(m_save_hostname).arg(m_save_port);
			emit signalCaption(buffer, m_save_encryption);
			menuView(VIEW_CHAT);
			if(m_save_loginmode == GGZCoreServer::firsttime)
			{
				KMessageBox::information(this,
					i18n("You are welcome as a new GGZ Gaming Zone player.\n"
						"Your personal password is: %1").arg(kggzserver->password()),
					i18n("Information"));
#ifdef KGGZ_WALLET
				w = KWallet::Wallet::openWallet("kggz");
				if(w)
				{
					entry = QString("%1@%2").arg(m_save_username).arg(m_save_hostname);
					if(!w->hasFolder("passwords"))
						w->createFolder("passwords");
					w->setFolder("passwords");
					w->writePassword(entry, kggzserver->password());
				}
				else KMessageBox::error(this,
					i18n("The wallet could not be opened to insert the password (%1).").arg(
						kggzserver->password()),
					i18n("Connection"));
#endif
			}
			buffer = i18n("Logged in as %1").arg(m_save_username);
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_INFO);
			m_workspace->widgetChat()->receive(NULL, i18n("Please join a room to start!"), KGGZChat::RECEIVE_INFO);
			if((m_save_loginmode == GGZCoreServer::firsttime) && (m_motd)) m_motd->raise();
			kggzserver->listRooms(-1, 1);
			if(kggzserver->listGames(1) != 0) // NEVER use 0 here, it will hang the client !!!
			{
				KGGZDEBUG("HUH? Don't give me game type list?!\n");
				return;
			}

			break;
		case GGZCoreServer::loginfail:
			KGGZDEBUG("loginfail\n");
			KMessageBox::error(this,
				i18n("Login refused! Reason: %1").arg(error->message),
				i18n("Error!"));
			m_killserver = 1;
			menuConnect();
			break;
		case GGZCoreServer::motdloaded:
			KGGZDEBUG("motdloaded\n");
			if(!m_motd) m_motd = new KGGZMotd(NULL, "KGGZMotd");
			motd = (GGZMotdEventData*)data;
			m_motd->setSource(motd->motd);
			m_motd->setWebpage(motd->url);
			if(m_showmotd) m_motd->show();
			break;
		case GGZCoreServer::roomlist:
			KGGZDEBUG("roomlist\n");
			break;
		case GGZCoreServer::typelist:
			KGGZDEBUG("typelist\n");

			emit signalMenu(MENUSIG_ROOMLIST);
			for(int i = 0; i < kggzserver->countRooms(); i++)
			{
				bool roominstalled = true;
				GGZCoreGametype *gametype = kggzserver->room(i)->gametype();
				if(gametype->name())
				{
					GGZCoreModule *module = new GGZCoreModule();
					module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
					if(module->count() == 0) roominstalled = false;
					delete module;
				}

				emit signalRoom(kggzserver->room(i)->name(),
					kggzserver->room(i)->gametype()->protocolEngine(),
					kggzserver->room(i)->category(),
					kggzserver->room(i)->countPlayers(),
					!kggzserver->room(i)->closed(),
					roominstalled);
				m_roommap[kggzserver->room(i)->gametype()->protocolEngine()] = i;
			}

			if(!kggzroom)
			{
				menuView(VIEW_CHAT);
				menuRoom(0);
			}
			break;
		case GGZCoreServer::entered:
			KGGZDEBUG("entered\n");
			eventLeaveRoom();
			kggzroom = kggzserver->room();
			attachRoomCallbacks();
			kggzroom->listPlayers();
			kggzroom->listTables(-1, 0);
			slotLoadLogo();
			buffer.sprintf(i18n("Entered room %s"), kggzroom->name());
			m_workspace->widgetChat()->init();
			m_workspace->widgetChat()->receive(NULL, buffer, KGGZChat::RECEIVE_ADMIN);
			if(kggzroom->description())
				m_workspace->widgetChat()->receive(NULL, i18n("(Description: %1)").arg(kggzroom->description()), KGGZChat::RECEIVE_ADMIN);
			m_workspace->widgetUsers()->setRoom(kggzroom);
			emit signalLocation(i18n("Room: ") + kggzroom->name());
			emit signalMenu(MENUSIG_ROOMENTER);

			gametype = kggzroom->gametype();
			KGGZDEBUG("Spectators allowed here? %i\n", gametype->maxSpectators());
			if(gametype->maxSpectators()) emit signalMenu(MENUSIG_SPECTATORS);
			else emit signalMenu(MENUSIG_NOSPECTATORS);

			if(gamerulesurl()) emit signalMenu(MENUSIG_RULES);
			else emit signalMenu(MENUSIG_NORULES);

			roomnumber = m_roommap[kggzroom->gametype()->protocolEngine()];
			emit signalRoomChanged(kggzroom->name(), kggzroom->gametype()->protocolEngine(),
				roomnumber, kggzroom->countPlayers() + 1); // FIXME: bug in ggzcore?
			break;
		case GGZCoreServer::enterfail:
			KGGZDEBUG("enterfail\n");
			KMessageBox::sorry(this, i18n("Sorry, you cannot change the room while playing!"), i18n("Room join failure"));
			break;
		case GGZCoreServer::loggedout:
			KGGZDEBUG("loggedout\n");
			delete m_sn_server;
			m_sn_server = NULL;
			m_workspace->widgetChat()->receive(NULL, i18n("Logged out"), KGGZChat::RECEIVE_ADMIN);
			m_killserver = 1;
			KGGZDEBUG("Disconnection successful.\n");
			emit signalMenu(MENUSIG_DISCONNECT);
			break;
		case GGZCoreServer::neterror:
			KGGZDEBUG("neterror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Network error detected!"), KGGZChat::RECEIVE_ADMIN);
			m_killserver = 1;
			menuConnect();
			break;
		case GGZCoreServer::protoerror:
			KGGZDEBUG("protoerror\n");
			m_workspace->widgetChat()->receive(NULL, i18n("ERROR: Protocol error detected!"), KGGZChat::RECEIVE_ADMIN);
			delete m_sn_server;
			m_sn_server = NULL;
			KMessageBox::error(this, i18n("A protocol error (GGZCoreServer::protoerror) occurred!"), i18n("Error!"));
			m_killserver = 1;
			menuConnect();
			break;
		case GGZCoreServer::chatfail:
			KGGZDEBUG("chatfail\n");
			m_workspace->widgetChat()->receive(NULL, i18n(QString("ERROR: Chat error detected: ").append((char*)data)), KGGZChat::RECEIVE_ADMIN);
			break;
		case GGZCoreServer::statechange:
			KGGZDEBUG("State is now: %s\n", KGGZCommon::state(kggzserver->state()));
			// Spades-type Workaround
			//if(kggzserver->state() == GGZ_STATE_IN_ROOM)
			//{
				//KGGZDEBUG(" -- Do we have a game? %i\n", kggzgame);
				//KGGZDEBUG(" -- Do we have a table? %i\n", kggzserver->isAtTable());
				//if(kggzroom) kggzroom->leaveTable();
			//}
			//if(kggzserver->state() == GGZ_STATE_IN_ROOM)
			//{
				//m_workspace->widgetLogo()->setErasePixmap(QPixmap(NULL));
			//}
			emit signalState(kggzserver->state());
			break;
		case GGZCoreServer::channelconnected:
			KGGZDEBUG("##### CHANNEL: connected!\n");
			m_channelfd = kggzserver->channel();
			break;
		case GGZCoreServer::channelready:
			KGGZDEBUG("##### CHANNEL: ready!\n");
			m_channelfd = -1;

			//game_channel_ready();
			if(kggzgame)
			{
				kggzgame->setFd(kggzserver->channel());
			}
			else
			{
				// in case of table leave due to problem, kggzgame might be NULL
				delete m_launch;
				m_launch = NULL;
				KMessageBox::error(this,
					i18n("Could not establish channel due to failed launch!"),
					i18n("Table launch error!"));
			}

			break;
		case GGZCoreServer::channelfail:
			KGGZDEBUG("##### CHANNEL: fail!\n");
			if(m_table)
			{
				delete m_table;
				m_table = NULL;
			}
			break;
		case GGZCoreServer::players:
			emit signalPlayers(kggzserver->countPlayers());
			break;
		case GGZCoreServer::rooms:
			KGGZDEBUG("[reconf] rooms changed\n");
			emit signalReconfiguration();
			for(int i = 0; i < kggzserver->countRooms(); i++)
			{
				bool roominstalled = true;
				GGZCoreGametype *gametype = kggzserver->room(i)->gametype();
				if(gametype->name())
				{
					GGZCoreModule *module = new GGZCoreModule();
					module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
					if(module->count() == 0) roominstalled = false;
					delete module;
				}

				emit signalRoom(kggzserver->room(i)->name(),
					kggzserver->room(i)->gametype()->protocolEngine(),
					kggzserver->room(i)->category(),
					kggzserver->room(i)->countPlayers(),
					!kggzserver->room(i)->closed(),
					roominstalled);
				m_roommap[kggzserver->room(i)->gametype()->protocolEngine()] = i;
			}
			break;
		default:
			KGGZDEBUG("unknown\n");
			break;
	}
}

GGZHookReturn KGGZ::hookOpenCollector(unsigned int id, const void* event_data, const void* user_data)
{
	KGGZCallback* kggzcallback;
	KGGZ* kggz;
	int opcode;

	kggzcallback = (KGGZCallback*)user_data;

	kggz = (KGGZ*)kggzcallback->pointer();
	opcode = kggzcallback->opcode();

	if(!kggz)
	{
		KGGZDEBUG("KGGZ is NULL!\n");
		return GGZ_HOOK_OK;
	}

	switch(opcode)
	{
		case COLLECTOR_SERVER:
			KGGZDEBUG("atom event: server\n");
			kggz->serverCollector(id, event_data);
			break;
		case COLLECTOR_ROOM:
			KGGZDEBUG("atom event: room\n");
			kggz->roomCollector(id, event_data);
			break;
		case COLLECTOR_GAME:
			KGGZDEBUG("atom event: game\n");
			kggz->gameCollector(id, event_data);
			break;
		default:
			KGGZDEBUG("atom event: unknown\n");
			break;
	}
	return GGZ_HOOK_OK;
}

void KGGZ::slotAdmin(int action, QString player, QString text)
{
	if((kggzserver) && (kggzroom))
	{
		KGGZDEBUG("Admin action on: %s\n", player.utf8().data());

		switch(action)
		{
			case KGGZChat::ADMIN_GAG:
				kggzroom->admin(GGZCoreRoom::admingag, player, NULL);
				break;
			case KGGZChat::ADMIN_UNGAG:
				kggzroom->admin(GGZCoreRoom::adminungag, player, NULL);
				break;
			case KGGZChat::ADMIN_KICK:
				kggzroom->admin(GGZCoreRoom::adminkick, player, text);
				break;
		}
	}
}

void KGGZ::slotChat(QString text, QString player, int mode)
{
	if((kggzserver) && (kggzroom))
	{
		KGGZDEBUG("Chat sends: %s\n", text.utf8().data());
		switch(mode)
		{
			case KGGZChat::RECEIVE_CHAT:
				kggzroom->chat(GGZCoreRoom::chatnormal, player, text);
				break;
			case KGGZChat::RECEIVE_TABLE:
				kggzroom->chat(GGZCoreRoom::chattable, player, text);
				break;
			case KGGZChat::RECEIVE_ANNOUNCE:
				if(kggzroom->chat(GGZCoreRoom::chatannounce, NULL, text) < 0)
				{
					m_workspace->widgetChat()->receive(NULL,
						i18n("Only administrators are allowed to broadcast messages."),
						KGGZChat::RECEIVE_ADMIN);
				}
				break;
			case KGGZChat::RECEIVE_BEEP:
				kggzroom->chat(GGZCoreRoom::chatbeep, player, text);
				break;
			case KGGZChat::RECEIVE_PERSONAL:
				kggzroom->chat(GGZCoreRoom::chatprivate, player, text);
				break;
		}
	}
	else KGGZDEBUG("Critical: No server or room found!\n");
}


void KGGZ::attachRoomCallbacks()
{
	KGGZDEBUG("== attaching hooks to room at %p\n", kggzroom);
	kggzroom->addHook(GGZCoreRoom::playerlist, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelist, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::chatevent, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::enter, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::leave, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableupdate, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelaunched, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablelaunchfail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablejoined, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tablejoinfail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableleft, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::tableleavefail, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	/*kggzroom->addHook(GGZCoreRoom::tabledata, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);*/
	kggzroom->addHook(GGZCoreRoom::stats, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
	kggzroom->addHook(GGZCoreRoom::count, &KGGZ::hookOpenCollector, (void*)kggzroomcallback);
}

void KGGZ::detachRoomCallbacks()
{
	KGGZDEBUG("== detaching hooks from room at %p\n", kggzroom);
	kggzroom->removeHook(GGZCoreRoom::playerlist, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelist, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::chatevent, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::enter, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::leave, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableupdate, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelaunched, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablelaunchfail, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablejoined, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tablejoinfail, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableleft, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::tableleavefail, &KGGZ::hookOpenCollector);
	/*kggzroom->removeHook(GGZCoreRoom::tabledata, &KGGZ::hookOpenCollector);*/
	kggzroom->removeHook(GGZCoreRoom::playerlag, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::stats, &KGGZ::hookOpenCollector);
	kggzroom->removeHook(GGZCoreRoom::count, &KGGZ::hookOpenCollector);
}

void KGGZ::attachServerCallbacks()
{
	KGGZDEBUG("=== attaching server hooks\n");
	kggzserver->addHook(GGZCoreServer::connected, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::connectfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::negotiated, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::negotiatefail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loggedin, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loginfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::motdloaded, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::roomlist, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::typelist, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::entered, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::enterfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::loggedout, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::neterror, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::protoerror, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::chatfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::statechange, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::channelconnected, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::channelready, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::channelfail, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::players, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
	kggzserver->addHook(GGZCoreServer::rooms, &KGGZ::hookOpenCollector, (void*)kggzservercallback);
}

void KGGZ::detachServerCallbacks()
{
	KGGZDEBUG("=== detaching hooks from server\n");
	kggzserver->removeHook(GGZCoreServer::connected, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::connectfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::negotiated, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::negotiatefail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loggedin, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loginfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::motdloaded, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::roomlist, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::typelist, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::entered, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::enterfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::loggedout, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::neterror, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::protoerror, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::chatfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::statechange, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::channelconnected, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::channelready, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::channelfail, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::players, &KGGZ::hookOpenCollector);
	kggzserver->removeHook(GGZCoreServer::rooms, &KGGZ::hookOpenCollector);
}

void KGGZ::attachGameCallbacks()
{
	KGGZDEBUG("=== attaching game hooks\n");
	kggzgame->addHook(GGZCoreGame::launched, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::launchfail, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::negotiated, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::negotiatefail, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
	kggzgame->addHook(GGZCoreGame::playing, &KGGZ::hookOpenCollector, (void*)kggzgamecallback);
}

void KGGZ::detachGameCallbacks()
{
	KGGZDEBUG("=== detaching hooks from game\n");
	kggzgame->removeHook(GGZCoreGame::launched, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::launchfail, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::negotiated, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::negotiatefail, &KGGZ::hookOpenCollector);
	kggzgame->removeHook(GGZCoreGame::playing, &KGGZ::hookOpenCollector);
}

void KGGZ::slotGameFire()
{
	KGGZDEBUG("KGGZ::slotGameFire() with frontend %i\n", m_gameinfo->frontend());

	slotLoadLogo();

	// not already at slotLaunchGame!
	emit signalMenu(MENUSIG_GAMESTART);
}

void KGGZ::menuView(int viewtype)
{
	m_workspace->show();
	m_splash->hide();
#ifdef KGGZ_BROWSER
	m_browser->hide();
#endif
	switch(viewtype)
	{
		case VIEW_CHAT:
			m_workspace->hideBar(0);
			m_workspace->widgetChat()->show();
			m_workspace->widgetLogo()->show();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->show();
			break;
		case VIEW_USERS:
			m_workspace->widgetChat()->hide();
			m_workspace->widgetUsers()->show();
			m_workspace->widgetLogo()->hide();
			m_workspace->widgetTables()->hide();
			m_workspace->hideBar(1);
			break;
		case VIEW_TABLES:
			m_workspace->hideBar(0);
			m_workspace->widgetChat()->hide();
			m_workspace->widgetLogo()->hide();
			m_workspace->widgetTables()->show();
			m_workspace->widgetUsers()->hide();
			break;
		case VIEW_SPLASH:
			m_splash->show();
			m_workspace->hide();
			break;
		case VIEW_BROWSER:
#ifdef KGGZ_BROWSER
			m_browser->show();
#endif
			m_workspace->hide();
			break;
		default:
			KGGZDEBUG("viewtype: unknown (%i)!\n", viewtype);
	}

	// Qt is crappy here... so we must do something for the chicks (animation)
	m_workspace->resize(width() - 1, height() - 1);
	m_workspace->resize(width(), height());
}

void KGGZ::menuGameLaunch()
{
	GGZCoreGametype *gametype;

	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("You cannot launch a game outside a room!"), i18n("Game launch failure"));
		return;
	}
	gametype = kggzroom->gametype();

	if(m_gameinfo) delete m_gameinfo;
	m_gameinfo = new KGGZGameInfo();
	m_gameinfo->setType(KGGZGameInfo::typelaunch);

	slotGameFrontend();
}

void KGGZ::menuGameJoin()
{
	int number;

	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("You must be in a room to join a table!"), i18n("Table join failure"));
		return;
	}
	number = m_workspace->widgetTables()->tablenum();
	if(number == -1)
	{
		if(kggzroom->countTables() == 1) number = 0;
		else
		{
			KMessageBox::sorry(this, i18n("Please select a table to join!"), i18n("Table join failure"));
			return;
		}
	}

	if(m_gameinfo) delete m_gameinfo;
	m_gameinfo = new KGGZGameInfo();
	m_gameinfo->setType(KGGZGameInfo::typejoin);
	m_gameinfo->setTable(number);

	slotGameFrontend();
}

void KGGZ::menuGameSpectator()
{
	int number;

	// FIXME: merge with menuGameJoin
	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("You must be in a room to join a table!"), i18n("Table join failure"));
		return;
	}
	number = m_workspace->widgetTables()->tablenum();
	if(number == -1)
	{
		if(kggzroom->countTables() == 1) number = 0;
		else
		{
			KMessageBox::sorry(this, i18n("Please select a table to join!"), i18n("Table join failure"));
			return;
		}
	}

	if(m_gameinfo) delete m_gameinfo;
	m_gameinfo = new KGGZGameInfo();
	m_gameinfo->setType(KGGZGameInfo::typespectator);
	m_gameinfo->setTable(number);

	slotGameFrontend();
}

void KGGZ::slotGameFrontend()
{
	GGZCoreGametype *gametype;
	int modules;
	int env;
	int ret;

	if((!kggzroom) || (!m_gameinfo))
	{
		KGGZDEBUG("Critical: Frontend selection without prior assignments\n");
		return;
	}

	gametype = kggzroom->gametype();

	m_core->reload();

	KGGZDEBUG("Create module...\n");
	m_module = new GGZCoreModule();
	m_module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	modules = m_module->count();

	KGGZDEBUG("Found: %i modules for this game\n", modules);
	if(modules == 0)
	{
		m_module->init(gametype->name(), NULL, gametype->protocolEngine());
		modules = m_module->count();
		if(modules == 0)
			KMessageBox::sorry(this, i18n("Sorry, no game modules found for this game type."), i18n("Game selection failure"));
		else
			KMessageBox::sorry(this,
				i18n(QString("The installed game modules for this game type are all incompatible.\n")) +
				i18n(QString("Please look for a game with the protocol engine '%1' and the protocol version '%2'.").arg(
				gametype->protocolEngine()).arg(gametype->protocolVersion())), i18n("Game selection failure"));
		delete m_module;
		m_module = NULL;
		return;
	}

	if(gametype->allowPeers())
	{
		ret = KMessageBox::questionYesNo(this,
			i18n("The selected game might reveal your IP address or hostname "
				"to other players. Do you still want to continue?"),
			i18n("Player Privacy"));
		if(ret != KMessageBox::Yes)
		{
			delete m_module;
			m_module = NULL;
			return;
		}
	}

	if(modules == 1) slotGamePrepare(0);
	else
	{
		if(m_selector) delete m_selector;
		m_selector = new KGGZSelector(NULL, "KGGZSelector");
		for(int i = 0; i < modules; i++)
		{
			m_module->setActive(i);
			env = m_module->environment();
			if((env == GGZCoreModule::xwindow) || (env == GGZCoreModule::xfullscreen))
				m_selector->addFrontend(m_module->game(), m_module->frontend(), i);
		}
		m_selector->show();
		connect(m_selector, SIGNAL(signalFrontend(int)), SLOT(slotGamePrepare(int)));
	}
}

void KGGZ::slotGamePrepare(int frontend)
{
	GGZCoreGametype *gametype;

	if(!kggzroom) return;
	gametype = kggzroom->gametype();

	m_gameinfo->setFrontend(frontend);
	if(m_gameinfo->type() == KGGZGameInfo::typejoin) slotGameStart();
	else if(m_gameinfo->type() == KGGZGameInfo::typespectator) slotGameStart();
	else
	{
		if(m_launch)
		{
			delete m_launch;
			m_launch = NULL;
		}
		if(!m_launch) m_launch = new KGGZLaunch(NULL, "KGGZLaunch");
		m_launch->initLauncher(m_save_username, gametype->maxPlayers(), gametype->maxBots());
		for(int i = 0; i < gametype->maxPlayers(); i++)
		{
			KGGZDEBUG("Assignment: %i is %i\n", i, gametype->isPlayersValid(i + 1));
			m_launch->setSeatAssignment(i, gametype->isPlayersValid(i + 1));
		}
		KGGZDEBUG("*** %i named bots\n", gametype->namedBots());
		for(int i = 0; i < gametype->namedBots(); i++)
		{
			m_launch->addBot(gametype->namedBotName(i), gametype->namedBotClass(i));
		}

		KSimpleConfig conf(QDir::home().path() + "/.ggz/kggz.rc");
		QMap<QString, QString> credits = conf.entryMap("Credits");
		QMap<QString, QString>::Iterator it;
		for(it = credits.begin(); it != credits.end(); it++)
		{
			int credit = it.data().toInt();
			if(credit == 1) // KGGZUsers::creditbuddy
			{
				m_launch->addBuddy(it.key());
			}
		}

		int number = kggzroom->countPlayers();
		for(int i = 0; i < number; i++)
		{
			GGZCorePlayer *player = kggzroom->player(i);
			QString playername = player->name();
			if(player->type() == KGGZUsers::assignbot)
			{
				m_launch->addBot(playername, "grubby");
			}
		}

		m_launch->show();
		connect(m_launch, SIGNAL(signalLaunch()), SLOT(slotGameStart()));
	}
}

void KGGZ::slotGameStart()
{
	int ret;

	m_module->setActive(m_gameinfo->frontend());
	if(!m_module->module())
	{
		KGGZDEBUG("null module!\n");
		return;
	}

	if(kggzgame)
	{
		KGGZDEBUG("Found game object alive; killing it now :-)\n");
		detachGameCallbacks();
		delete kggzgame;
	}

	kggzgame = new GGZCoreGame();
	kggzgame->init(kggzserver->server(), m_module->module());

	attachGameCallbacks();

	delete m_module;
	m_module = NULL;

	ret = kggzgame->launch();
	if(ret < 0)
	{
		KGGZDEBUG("Red Alert! Game launching failed immediately!\n");
		detachGameCallbacks();
		delete kggzgame;
		kggzgame = NULL;
		eventLeaveTable(1);
		KMessageBox::error(this, i18n("Couldn't launch game!"), i18n("Error!"));
		return;
	}
}

void KGGZ::menuGameInfo()
{
	GGZCoreGametype *gametype;
	GGZCoreModule *module;
	QString buffer, buffer2;

	if(!kggzserver)
		return;
	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("Please join a room first."), i18n("Game information failure"));
		return;
	}
	gametype = kggzroom->gametype();
	if(!gametype)
		return;
	if(!gametype->name())
	{
		KMessageBox::information(this, i18n("This is the GGZ Gaming Zone Lounge. You can meet other players here."), "Lounge");
		return;
	}

	buffer.append(QString("<b>%1</b>").arg(i18n("Game server")));
	buffer.append("<br>");
	buffer.append(i18n("Name: "));
	buffer.append(gametype->name());
	buffer.append("<br>");
	buffer.append(i18n("Description: "));
	buffer.append(gametype->description());
	buffer.append("<br>");
	buffer.append(i18n("Author: "));
	buffer.append(gametype->author());
	buffer.append("<br>");
	buffer.append(i18n("Version: "));
	buffer.append(gametype->version());
	buffer.append("<br>");
	buffer.append(i18n("Protocol: "));
	buffer.append(gametype->protocolVersion());
	buffer.append("<br>");
	buffer.append(i18n("Engine: "));
	buffer.append(gametype->protocolEngine());
	buffer.append("<br>");
	buffer.append(i18n("URL: "));
	buffer.append(QString("<a href=\"%1\">%2</a>").arg(gametype->url()).arg(gametype->url()));
	buffer.append("<br>");
	buffer.append(i18n("Game Category: "));
	buffer.append(gametype->category());
	buffer.append("<br>");
	buffer.append(i18n("Rating: "));
	buffer.append(gametype->rating());
	buffer.append("<br><br>");

	module = new GGZCoreModule();
	module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	for(unsigned int i = 0; i < module->count(); i++)
	{
		module->setActive(i);
		buffer.append(QString("<b>%1</b>").arg(i18n("Game client")));
		buffer.append("<br>");
		buffer.append(i18n("Name: "));
		buffer.append(module->game());
		buffer.append("<br>");
		buffer.append(i18n("Frontend: "));
		buffer.append(module->frontend());
		buffer.append("<br>");
		buffer.append(i18n("Author: "));
		buffer.append(module->author());
		buffer.append("<br>");
		buffer.append(i18n("URL: "));
		buffer.append(QString("<a href=\"%1\">%2</a>").arg(module->url()).arg(module->url()));
		buffer.append("<br><br>");

	}
	delete module;

	if(!m_gameinfodialog) m_gameinfodialog = new KGGZGameInfoDialog(NULL);
	m_gameinfodialog->setInformation(buffer);
	m_gameinfodialog->show();
}

QString KGGZ::gamerulesurl()
{
	GGZCoreGametype *gametype = kggzroom->gametype();
	kapp->config()->setGroup("Rules");
	QString rulesurl = kapp->config()->readEntry(gametype->name());
	return rulesurl;
}

void KGGZ::menuGameRules()
{
	GGZCoreGametype *gametype;
	if(!kggzserver)
		return;
	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("Please join a room first."), i18n("Game information failure"));
		return;
	}
	gametype = kggzroom->gametype();
	if(!gametype)
		return;

#ifdef KGGZ_BROWSER
	m_browser->request(gamerulesurl());
	menuView(VIEW_BROWSER);
#else
	kapp->invokeBrowser(gamerulesurl());
#endif
}

void KGGZ::menuRoom(int room)
{
	//int id;

	if(room >= 0)
	{
		if(kggzserver)
		{
			GGZCoreRoom *r = kggzserver->room(room);
			//id = r->id();
			GGZRoom *ggzroom = r->room();

			kggzserver->joinRoom(/*id*/ggzroom);
			if((m_workspace) && (m_workspace->widgetChat()))
			{
				m_workspace->widgetChat()->init();
				m_workspace->show();
			}
			else KGGZDEBUG("Critical: Workspace/Chat absent!\n");
		}
		else KGGZDEBUG("Critical: No server found.\n");
	}
}

void KGGZ::slotLoadLogo()
{
	GGZCoreModule *module;
	GGZCoreGametype *gametype;
	const char *icon;

	KGGZDEBUG("__ loading logo __\n");
	if(!kggzroom) return;
	gametype = kggzroom->gametype();
	if((!gametype) || (!gametype->name()))
	{
		m_workspace->widgetLogo()->setLogo(NULL, "lounge", true);
		return;
	}
	KGGZDEBUG("__ more loading: %s %s %s\n", gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());

	m_core->reload();

	module = new GGZCoreModule();
	module->init(gametype->name(), gametype->protocolVersion(), gametype->protocolEngine());
	if(module->count() == 0)
	{
		m_workspace->widgetLogo()->setLogo(NULL, gametype->protocolEngine(), false);
		delete module;
		return;
	}
	module->setActive(0);

	icon = module->pathIcon();
	m_workspace->widgetLogo()->setLogo(icon, gametype->protocolEngine(), true);

	delete module;
}

void KGGZ::menuGrubby()
{
	if(!kggzroom)
	{
		KMessageBox::sorry(this, i18n("Please join a room first."), i18n("Grubby failure"));
		return;
	}
	if(m_grubby) delete m_grubby;
	m_grubby = new KGGZGrubby(NULL, "KGGZGrubby");
	m_grubby->show();
	listPlayers();
	listTables();
	connect(m_grubby, SIGNAL(signalAction(QString, QString, int)), SLOT(slotGrubby(QString, QString, int)));
}

void KGGZ::slotGrubby(QString grubby, QString argument, int id)
{
	switch(id)
	{
		case KGGZGrubby::actionabout:
			slotChat("about", grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionhelp:
			slotChat("help", grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionseen:
			slotChat(QString("have you seen %1").arg(argument), grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionalertadd:
			slotChat(QString("alert %1").arg(argument), grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionmessages:
			slotChat("do i have any messages", grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionbye:
			KMessageBox::information(m_grubby, i18n("The pleasure was mine :)"), "Grubby");
			break;
		case KGGZGrubby::actionteach:
			slotChat(QString("%1").arg(argument), grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		case KGGZGrubby::actionwhois:
			slotChat(QString("who is %1").arg(argument), grubby, KGGZChat::RECEIVE_PERSONAL);
			break;
		default:
			KMessageBox::sorry(m_grubby, i18n("I don't know that command, sorry."), i18n("Grubby communication failure"));
	}
}

void KGGZ::menuPreferencesSettings()
{
	if(m_prefenv) delete m_prefenv;
	m_prefenv = new KGGZPrefEnv(NULL, "KGGZPrefEnv");
	connect(m_prefenv, SIGNAL(signalAccepted()), SLOT(slotAcceptedConfiguration()));
	m_prefenv->show();
}

void KGGZ::slotAcceptedConfiguration()
{
	m_prefenv->hide();
	readConfiguration(false);
}

void KGGZ::eventLeaveGame()
{
	if(!kggzgame)
	{
		KGGZDEBUG("Hrmpf, someone deleted kggzgame...\n");
	}
	else
	{
		detachGameCallbacks();
		delete kggzgame;
		kggzgame = NULL;
		m_workspace->widgetChat()->receive(NULL, i18n("Game over"), KGGZChat::RECEIVE_ADMIN);
	}
}

void KGGZ::eventLeaveTable(int force)
{
	KGGZDEBUG("Leave table?\n");
	if((kggzserver) && (kggzserver->state() == GGZ_STATE_AT_TABLE))
	{
		KGGZDEBUG("**** Still at table (alert) -> leaving now with force %i!\n", force);
		if(m_gameinfo->type() == KGGZGameInfo::typespectator) kggzroom->leaveTableSpectator();
		else kggzroom->leaveTable(force);
	}
}

void KGGZ::eventLeaveRoom()
{
	if(!kggzroom)
	{
		KGGZDEBUG("Pffft... no room here.\n");
		return;
	}
	detachRoomCallbacks();
	delete kggzroom;
	kggzroom = NULL;
	m_workspace->widgetChat()->chatline()->removeAll();
	m_workspace->widgetUsers()->removeall();
	m_workspace->widgetTables()->reset();
	m_workspace->widgetChat()->shutdown();
	m_workspace->widgetLogo()->shutdown();
	m_workspace->widgetUsers()->setRoom(NULL);
	emit signalLocation(i18n("No room selected"));
}

void KGGZ::menuGameCancel()
{
	KGGZDEBUGF("KGGZ::menuGameCancel()\n");
	eventLeaveTable(0);
}

void KGGZ::menuGameTeam()
{
	if(!m_team) m_team = new KGGZTeam();
	m_team->show();
}

void KGGZ::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);

	emit signalActivity(ACTIVITY_NONE);
}

