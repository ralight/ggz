#include "vencedor.h"

#include "prefs.h"

#include <QLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QCheckBox>

#include <klocale.h>
//#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kaboutapplicationdialog.h>
#include <kcomponentdata.h>
#include <kconfigdialog.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>

//#include <kggzcore/room.h>
#include <kggzcore/misc.h>

#include <kggzlib/room.h>
#include <kggzlib/roomlist.h>
#include <kggzlib/player.h>
#include <kggzlib/playerlist.h>
#include <kggzlib/tablelist.h>
#include <kggzlib/tabledialog.h>
//#include <kggzlib/kggzaction.h>
#include <kggzlib/connectiondialog.h>
#include <kggzlib/connectionprofiles.h>
#include <kggzlib/moduledialog.h>
#include <kggzlib/kggzcorelayer.h>

#include <kchat.h>

Vencedor::Vencedor(QString url)
: QMainWindow()
{
	KStandardDirs d;

	QWidget *centralwidget = new QWidget();
	setCentralWidget(centralwidget);

	m_rooms = new RoomList();
	m_players = new PlayerList();
	m_tables = new TableList();
	m_chat = new KChat(centralwidget, false);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_rooms);
	vbox->addWidget(m_players);
	vbox->addWidget(m_tables);

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addWidget(m_chat);

	centralwidget->setLayout(hbox);

	QToolBar *toolbar = new QToolBar();
	addToolBar(Qt::TopToolBarArea, toolbar);

	// FIXME: This is too bizarre!
	//toolbar->addAction(kggzAction(QString(), this, NULL, KActionCollection(), "connect-ggz"));
	QString ggzicon = d.findResource("data", "kggzlib/icons/ggz.png");
	m_action_connect = new QAction(QIcon(ggzicon), i18n("Connect to GGZ Gaming Zone"), this);
	connect(m_action_connect, SIGNAL(triggered(bool)), SLOT(slotConnect()));
	toolbar->addAction(m_action_connect);

	QPixmap icon_disconnect = KIconLoader::global()->loadIcon("network-disconnect", KIconLoader::Small);
	m_action_disconnect = new QAction(QIcon(icon_disconnect), i18n("Disconnect"), this);
	connect(m_action_disconnect, SIGNAL(triggered(bool)), SLOT(slotDisconnect()));
	toolbar->addAction(m_action_disconnect);

	toolbar->addSeparator();

	QPixmap icon_help = KIconLoader::global()->loadIcon("help-contents", KIconLoader::Small);
	QAction *action_help = new QAction(QIcon(icon_help), i18n("Help"), this);
	connect(action_help, SIGNAL(triggered(bool)), SLOT(slotHelp()));
	toolbar->addAction(action_help);

	QPixmap icon_about = KIconLoader::global()->loadIcon("help-about", KIconLoader::Small);
	QAction *action_about = new QAction(QIcon(icon_about), i18n("About Vencedor"), this);
	connect(action_about, SIGNAL(triggered(bool)), SLOT(slotAbout()));
	toolbar->addAction(action_about);

	QPixmap icon_modules = KIconLoader::global()->loadIcon("help-hint", KIconLoader::Small);
	QAction *action_modules = new QAction(QIcon(icon_modules), i18n("Game Modules"), this);
	connect(action_modules, SIGNAL(triggered(bool)), SLOT(slotModules()));
	toolbar->addAction(action_modules);

	QPixmap icon_config = KIconLoader::global()->loadIcon("configure", KIconLoader::Small);
	QAction *action_config = new QAction(QIcon(icon_config), i18n("Configuration"), this);
	connect(action_config, SIGNAL(triggered(bool)), SLOT(slotConfig()));
	toolbar->addAction(action_config);

	QPixmap icon_quit = KIconLoader::global()->loadIcon("application-exit", KIconLoader::Small);
	QAction *action_quit = new QAction(QIcon(icon_quit), i18n("Quit"), this);
	connect(action_quit, SIGNAL(triggered(bool)), SLOT(close()));
	toolbar->addAction(action_quit);

	toolbar->addSeparator();

	QPixmap icon_launch = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	m_action_launch = new QAction(QIcon(icon_launch), i18n("Launch a new game"), this);
	connect(m_action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
	m_action_launch->setEnabled(false);
	toolbar->addAction(m_action_launch);

	connect(m_chat, SIGNAL(signalSendMessage(int, const QString)), SLOT(slotChatEntered(int, const QString&)));
	connect(m_rooms, SIGNAL(signalSelected(const QString&)), SLOT(slotRoom(const QString)));
	connect(m_rooms, SIGNAL(signalFavourite(const QString&, bool)), SLOT(slotFavourite(const QString, bool)));

	enable(false);

	setWindowTitle(i18n("Vencedor"));
	resize(800, 700);
	show();

	if(!url.isEmpty())
		connection(url);
	else if(Prefs::autoconnect())
		connection(url);
	// FIXME: autoconnect() should work on previously used profile
}

void Vencedor::connection(const QString& url)
{
	// FIXME: This is currently working differently from the dialogue-driven connection
	// FIXME: We need to use kggzcorelayer here, too

	// FIXME: should recognise if username is missing but host/port are given
	// FIXME: guest mode unless URI is known to refer to registered profile
	ConnectionProfiles cp(this);
	// cp.setMetaserver(...);
	cp.presetServer(url);
	if(cp.exec() == QDialog::Accepted)
	{
		QList<GGZProfile> profiles = cp.profiles();
		// ...
	}
	else
	{
		return;
	}

	m_action_connect->setEnabled(false);

	m_core = new KGGZCore::CoreClient(this);

	connect(m_core,
		SIGNAL(signalFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)),
		SLOT(slotFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)));
	connect(m_core,
		SIGNAL(signalAnswer(KGGZCore::CoreClient::AnswerMessage)),
		SLOT(slotAnswer(KGGZCore::CoreClient::AnswerMessage)));
	connect(m_core,
		SIGNAL(signalEvent(KGGZCore::CoreClient::EventMessage)),
		SLOT(slotEvent(KGGZCore::CoreClient::EventMessage)));

	m_core->setUrl(url);
	m_core->setTLS(Prefs::enforcetls());
	m_core->initiateLogin();
}

void Vencedor::slotFavourite(const QString& roomname, bool favourite)
{
	KConfigGroup favgroup = KGlobal::config()->group("Favourites");
	favgroup.writeEntry(roomname, favourite);
}

void Vencedor::slotAbout()
{
	const KComponentData component = KGlobal::mainComponent();
	const KAboutData *about = component.aboutData();
	KAboutApplicationDialog dlg(about, this);
	dlg.exec();
}

void Vencedor::slotHelp()
{
	KToolInvocation::invokeBrowser("help:/vencedor/index.html");
}

void Vencedor::slotModules()
{
	ModuleDialog dlg(this);
	dlg.exec();
}

void Vencedor::slotConfig()
{
	if(KConfigDialog::showDialog("settings"))
		return;

	KConfigDialog *dialog = new KConfigDialog(this, "settings", Prefs::self());

	QWidget *root = new QWidget();

	QCheckBox *kcfg_sync = new QCheckBox(i18n("Synchronise preferences with server"));
	QCheckBox *kcfg_autoconnect = new QCheckBox(i18n("Automatically connect to previous session"));
	QCheckBox *kcfg_enforcetls = new QCheckBox(i18n("Require encryption and reject unencrypted servers"));

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(kcfg_sync);
	vbox->addWidget(kcfg_autoconnect);
	vbox->addWidget(kcfg_enforcetls);

	root->setLayout(vbox);

	dialog->addPage(root, i18n("Settings"), "ggz");

	dialog->show();
}

void Vencedor::slotDisconnect()
{
	enable(false);
	delete m_core;
	m_core = NULL;
}

void Vencedor::slotConnect()
{
	ConnectionDialog dlg(this);
	dlg.setCoreMode(true);
	if(dlg.exec() == QDialog::Accepted)
	{
		// FIXME: the instance in connectiondialog is not ours
		// FIXME: rather crude sharing, must switch to kggzcorelayer entirely!
		m_core = dlg.core();
		enable(true);
		handleRoomlist();
		handleSession();

		// FIXME: This part is now called after we enter the first room
		// In the event of this room having a game attached, we wouldn't run 'our' room_entered hook
		connect(m_core,
			SIGNAL(signalFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)),
			SLOT(slotFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)));
		connect(m_core,
			SIGNAL(signalAnswer(KGGZCore::CoreClient::AnswerMessage)),
			SLOT(slotAnswer(KGGZCore::CoreClient::AnswerMessage)));
		connect(m_core,
			SIGNAL(signalEvent(KGGZCore::CoreClient::EventMessage)),
			SLOT(slotEvent(KGGZCore::CoreClient::EventMessage)));
	}
}

void Vencedor::handleSession()
{
	m_players->setSelf(m_core->username());

	// FIXME: Where to get profile from?
	GGZServer server;
	server.setApi("http://api.ggzcommunity.local/");
	GGZProfile profile;
	profile.setGGZServer(server);
	profile.setUsername(m_core->username());
	profile.setPassword(m_core->password());
	m_players->setGGZProfile(profile);
	m_rooms->setGGZProfile(profile);
}

void Vencedor::slotLaunch()
{
	TableDialog tabledlg(this);
	// FIXME: see ConnectionDialog.cpp
	KGGZCore::GameType gametype = m_core->room()->gametype();
	tabledlg.setGameType(gametype);
	tabledlg.setIdentity(m_core->username());
	if(tabledlg.exec() == QDialog::Accepted)
	{
		KGGZCoreLayer *corelayer = new KGGZCoreLayer(this);
		corelayer->setCore(m_core);
		corelayer->configureTable(tabledlg.table().players());
		corelayer->launch();
	}
}

void Vencedor::slotChatEntered(int id, const QString& msg)
{
	Q_UNUSED(id);
	Q_UNUSED(msg);

	kDebug() << "CHAT:" << id << msg;
	m_core->room()->sendchat(msg, QString(), KGGZCore::Room::chatnormal);
}

void Vencedor::slotRoom(const QString& name)
{
	m_core->initiateRoomChange(name);
}

void Vencedor::enable(bool enabled)
{
	m_rooms->setEnabled(enabled);
	m_players->setEnabled(enabled);
	m_tables->setEnabled(enabled);
	m_chat->setEnabled(enabled);
	m_action_connect->setEnabled(!enabled);
	m_action_disconnect->setEnabled(enabled);
}

void Vencedor::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	//KGGZCore::Room *room;

	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			if(error == KGGZCore::Error::generic_error)
			{
				KMessageBox::error(this,
					i18n("Unable to connect."),
					i18n("Connection"));
				slotConnect();
			}
			break;
		case KGGZCore::CoreClient::negotiation:
			if(error == KGGZCore::Error::generic_error)
			{
				KMessageBox::error(this,
					i18n("Wrong protocol."),
					i18n("Connection"));
				slotConnect();
			}
			break;
		case KGGZCore::CoreClient::login:
			if(error == KGGZCore::Error::generic_error)
			{
				KMessageBox::error(this,
					i18n("Unable to login."),
					i18n("Connection"));
				slotConnect();
			}
			else
			{
				handleSession();
			}
			break;
		case KGGZCore::CoreClient::roomenter:
			//if(error == KGGZCore::Error::no_status)
			//room = m_core->room();
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

			if(!m_core->room()->gametype().name().isEmpty())
				m_action_launch->setEnabled(true);
			else
				m_action_launch->setEnabled(false);
			enable(true);
			break;
		case KGGZCore::CoreClient::chat:
			// FIXME: how can we access the chat message here?
			// FIXME: a separate signal would probably be the best idea...
			break;
		case KGGZCore::CoreClient::channel:
			break;
		case KGGZCore::CoreClient::logout:
			break;
	}
}

void Vencedor::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			handleRoomlist();
			break;
		case KGGZCore::CoreClient::typelist:
			break;
		case KGGZCore::CoreClient::motd:
			qDebug("MOTD web url: %s", qPrintable(m_core->webmotd()));
			qDebug("MOTD web text:\n%s", qPrintable(m_core->textmotd()));
			break;
	}
}

void Vencedor::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::state_changed:
			qDebug("* state: %s", qPrintable(KGGZCore::Misc::statename(m_core->state())));
			break;
		case KGGZCore::CoreClient::players_changed:
			break;
		case KGGZCore::CoreClient::rooms_changed:
			break;
		case KGGZCore::CoreClient::neterror:
			break;
		case KGGZCore::CoreClient::protoerror:
			break;
		case KGGZCore::CoreClient::libraryerror:
			enable(false);
			break;
	}
}

void Vencedor::slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
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

void Vencedor::slotAnswer(KGGZCore::Room::AnswerMessage message)
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
			tables = m_core->room()->tables();
			for(int i = 0; i < tables.count(); i++)
			{
				KGGZCore::Table table = tables.at(i);
				m_tables->addConfiguration(table);
			}
			break;
	}
}

void Vencedor::slotEvent(KGGZCore::Room::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::chat:
			//chat(i18n("Players are chatting..."));
			break;
		case KGGZCore::Room::enter:
			break;
		case KGGZCore::Room::leave:
			break;
		case KGGZCore::Room::tableupdate:
			//qDebug() << "#tables(update)";
			//checkTables();
			//roominfo();
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

void Vencedor::slotChat(QString sender, QString message, KGGZCore::Room::ChatType type)
{
	if(type == KGGZCore::Room::chatprivate)
		m_chat->addSystemMessage(sender, message);
	else
		m_chat->addMessage(sender, message);
}

void Vencedor::handleRoomlist()
{
	KConfigGroup favgroup = KGlobal::config()->group("Favourites");

	m_core->initiateRoomChange(m_core->roomnames().at(0));
	for(int i = 0; i < m_core->roomnames().count(); i++)
	{
		QString roomname = m_core->roomnames().at(i);
		bool fav = favgroup.readEntry(roomname, false);

		Room *room = new Room(roomname);
		room->setPlayers(0);
		room->setFavourite(fav);
		m_rooms->addRoom(room);
	}
}

