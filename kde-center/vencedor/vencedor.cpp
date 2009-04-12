#include "vencedor.h"

#include "prefs.h"
#include "motd.h"

#include <QLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QCheckBox>
#include <QApplication>
#include <QComboBox>
#include <QLabel>

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
#include <kcodecs.h>

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
#include <kggzlib/connectionsingle.h>
#include <kggzlib/moduledialog.h>
#include <kggzlib/kggzcorelayer.h>
#include <kggzlib/embeddedcoreclient.h>

#include <kchat.h>

Vencedor::Vencedor(QString url)
: QMainWindow()
{
	KStandardDirs d;

	m_ecc = new EmbeddedCoreClient(NULL, true);

	QWidget *centralwidget = new QWidget();
	setCentralWidget(centralwidget);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_ecc->widget_rooms());
	vbox->addWidget(m_ecc->widget_players());
	vbox->addWidget(m_ecc->widget_tables());

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addWidget(m_ecc->widget_chat());

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

	QPixmap icon_motd = KIconLoader::global()->loadIcon("mail-mark-read", KIconLoader::Small);
	m_action_motd = new QAction(QIcon(icon_motd), i18n("Message of the day"), this);
	connect(m_action_motd, SIGNAL(triggered(bool)), SLOT(slotMotd()));
	toolbar->addAction(m_action_motd);

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

	connect(m_ecc->action_launch(), SIGNAL(triggered(bool)), SLOT(slotLaunch()));
	m_ecc->action_launch()->setEnabled(false);
	toolbar->addAction(m_ecc->action_launch());

	connect(m_ecc->action_join(), SIGNAL(triggered(bool)), SLOT(slotJoin()));
	toolbar->addAction(m_ecc->action_join());

	connect(m_ecc->action_spectate(), SIGNAL(triggered(bool)), SLOT(slotSpectate()));
	toolbar->addAction(m_ecc->action_spectate());

	connect(m_ecc->widget_chat(), SIGNAL(signalSendMessage(int, const QString)), SLOT(slotChatEntered(int, const QString&)));
	connect(m_ecc->widget_rooms(), SIGNAL(signalSelected(const QString&)), SLOT(slotRoom(const QString&)));
	connect(m_ecc->widget_rooms(), SIGNAL(signalFavourite(const QString&, bool)), SLOT(slotFavourite(const QString&, bool)));

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
	GGZProfile profile;
	ConnectionSingle cs(this);
	// cp.setMetaserver(...);
	cs.presetServer(url);
	if(cs.exec() == QDialog::Accepted)
	{
		profile = cs.profile();
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

	QUrl uri = profile.ggzServer().uri();
	uri.setUserName(profile.username());
	uri.setPassword(profile.password());
	uri.setPath(profile.roomname());
	QString customurl = uri.toString();

	KGGZCore::CoreClient::LoginType loginmode = KGGZCore::CoreClient::normal;
	if(profile.loginType() == GGZProfile::guest)
		loginmode = KGGZCore::CoreClient::guest;
	else if(profile.loginType() == GGZProfile::firsttime)
		loginmode = KGGZCore::CoreClient::firsttime;

	m_core->setUrl(customurl);
	m_core->setEmail(profile.email());
	m_core->setMode(loginmode);
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
	QComboBox *kcfg_motd = new QComboBox();
	kcfg_motd->addItem(i18n("Always"), Prefs::EnumMotddisplay::always);
	kcfg_motd->addItem(i18n("Never"), Prefs::EnumMotddisplay::never);
	kcfg_motd->addItem(i18n("New messages only"), Prefs::EnumMotddisplay::newonly);
	QLabel *label_motd = new QLabel(i18n("Display of the Message Of The Day (MOTD)"));

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(kcfg_sync);
	vbox->addWidget(kcfg_autoconnect);
	vbox->addWidget(kcfg_enforcetls);
	vbox->addWidget(label_motd);
	vbox->addWidget(kcfg_motd);

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
	m_ecc->widget_players()->setSelf(m_core->username());

	// FIXME: Where to get profile from?
	GGZServer server;
	server.setApi("http://api.ggzcommunity.local/");
	GGZProfile profile;
	profile.setGGZServer(server);
	profile.setUsername(m_core->username());
	profile.setPassword(m_core->password());
	m_ecc->widget_players()->setGGZProfile(profile);
	m_ecc->widget_rooms()->setGGZProfile(profile);
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
		corelayer->configureTable(tabledlg.table().description(), tabledlg.table().players());
		corelayer->launch();
	}
}

void Vencedor::slotChatEntered(int id, const QString& msg)
{
	Q_UNUSED(id);

	kDebug() << "CHAT:" << id << msg;
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

void Vencedor::slotRoom(const QString& name)
{
	m_core->initiateRoomChange(name);
}

void Vencedor::slotMotd()
{
	Motd motd(this);
	motd.setText(m_core->textmotd());
	motd.setWebpage(m_core->webmotd());
	motd.exec();
}

void Vencedor::enable(bool enabled)
{
	if(!enabled)
		m_action_motd->setEnabled(enabled);

	m_ecc->widget_rooms()->setEnabled(enabled);
	m_ecc->widget_players()->setEnabled(enabled);
	m_ecc->widget_tables()->setEnabled(enabled);
	m_ecc->widget_chat()->setEnabled(enabled);
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
				m_ecc->action_launch()->setEnabled(true);
			else
				m_ecc->action_launch()->setEnabled(false);
			enable(true);
			m_ecc->widget_chat()->addSystemMessage(i18n("Vencedor"), i18n("Entered room %1.").arg(m_core->roomname()));
			m_ecc->widget_rooms()->select(m_core->roomname());
			break;
		case KGGZCore::CoreClient::chat:
			// FIXME: This is only chat failure, see room chat for incoming messages
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
			bool display = false;
			if(Prefs::motddisplay() == Prefs::EnumMotddisplay::always)
			{
				display = true;
			}
			else if(Prefs::motddisplay() == Prefs::EnumMotddisplay::newonly)
			{
				KMD5 md5text(m_core->textmotd().toUtf8());
				KMD5 md5web(m_core->webmotd().toUtf8());
				QString md5sumtext = md5text.hexDigest().data();
				QString md5sumweb = md5web.hexDigest().data();

				KConfigGroup motdgroup = KGlobal::config()->group("MOTD");
				QString md5sumtextold = motdgroup.readEntry("md5sumtext", QString());
				QString md5sumwebold = motdgroup.readEntry("md5sumweb", QString());
				if((md5sumtext != md5sumtextold) || (md5sumweb != md5sumwebold))
				{
					motdgroup.writeEntry("md5sumtext", md5sumtext);
					motdgroup.writeEntry("md5sumweb", md5sumweb);
					display = true;
				}
			}
			m_action_motd->setEnabled(true);
			if(display)
				slotMotd();
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
			m_ecc->widget_players()->clear();
			players = m_core->room()->players();
			for(int i = 0; i < players.count(); i++)
			{
				KGGZCore::Player player = players.at(i);
				Player *p = new Player(player.name());
				m_ecc->widget_players()->addPlayer(p);
			}
			break;
		case KGGZCore::Room::tablelist:
			m_ecc->widget_tables()->clear();
			// FIXME: shouldn't be necessary to enforce the disabled join/spectate buttons
			// FIXME: needs to be integrated with EmbeddedCoreClient now, e.g. setTables(tables)
			//slotTable(KGGZCore::Table(), -1);
			tables = m_core->room()->tables();
			for(int i = 0; i < tables.count(); i++)
			{
				KGGZCore::Table table = tables.at(i);
				m_ecc->widget_tables()->addConfiguration(table);
			}
			break;
	}
}

void Vencedor::slotEvent(KGGZCore::Room::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::chat:
			// FIXME: this is now in slotChat()
			break;
		case KGGZCore::Room::enter:
			m_ecc->widget_chat()->addSystemMessage(i18n("Vencedor"), i18n("--> %1 has entered the room.").arg("SOMEBODY"));
			break;
		case KGGZCore::Room::leave:
			m_ecc->widget_chat()->addSystemMessage(i18n("Vencedor"), i18n("<-- %1 has left the room.").arg("SOMEBODY"));
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
	{
		message.prepend("[PRIVATE] ");
	}
	else if(type == KGGZCore::Room::chatbeep)
	{
		QApplication::beep();
		message = "[BEEP]";
	}

	// FIXME: colours and style?
	QFont namefont = m_ecc->widget_chat()->nameFont();
	QFont smfont = m_ecc->widget_chat()->systemMessageFont();
	namefont.setItalic(true);
	smfont.setBold(true);
	m_ecc->widget_chat()->setNameFont(namefont);
	m_ecc->widget_chat()->setSystemMessageFont(smfont);

	if(type != KGGZCore::Room::chatnormal)
		m_ecc->widget_chat()->addSystemMessage(sender, message);
	else
		m_ecc->widget_chat()->addMessage(sender, message);
}

void Vencedor::handleRoomlist()
{
	KConfigGroup favgroup = KGlobal::config()->group("Favourites");

	for(int i = 0; i < m_core->roomnames().count(); i++)
	{
		QString roomname = m_core->roomnames().at(i);
		bool fav = favgroup.readEntry(roomname, false);

		Room *room = new Room(roomname);
		room->setPlayers(0);
		room->setFavourite(fav);
		m_ecc->widget_rooms()->addRoom(room);
	}

	QString joinroom = m_core->roomname();
	if(joinroom.isEmpty())
		joinroom = m_core->roomnames().at(0);

	m_core->initiateRoomChange(joinroom);
}

