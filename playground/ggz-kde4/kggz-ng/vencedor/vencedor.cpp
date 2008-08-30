#include "vencedor.h"

#include <QLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>

#include <klocale.h>
//#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kaboutapplicationdialog.h>
#include <kcomponentdata.h>

//#include <kggzcore/room.h>
#include <kggzcore/misc.h>

#include <kggzlib/room.h>
#include <kggzlib/roomlist.h>
#include <kggzlib/player.h>
#include <kggzlib/playerlist.h>
#include <kggzlib/tablelist.h>
//#include <kggzlib/kggzaction.h>
#include <kggzlib/connectiondialog.h>

#include <kchat.h>

Vencedor::Vencedor(QString url)
: QMainWindow()
{
	KStandardDirs d;

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
	m_core->initiateLogin();

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
	QString ggzicon = d.findResource("data", "kggzlib/games/chess.png");
	QAction *action_connect = new QAction(QIcon(ggzicon), i18n("Connect to GGZ Gaming Zone"), this);
	connect(action_connect, SIGNAL(triggered(bool)), SLOT(slotConnect()));
	toolbar->addAction(action_connect);

	QPixmap icon_about = KIconLoader::global()->loadIcon("help-about", KIconLoader::Small);
	QAction *action_about = new QAction(QIcon(icon_about), i18n("About Vencedor"), this);
	connect(action_about, SIGNAL(triggered(bool)), SLOT(slotAbout()));
	toolbar->addAction(action_about);

	QPixmap icon_quit = KIconLoader::global()->loadIcon("application-exit", KIconLoader::Small);
	QAction *action_quit = new QAction(QIcon(icon_quit), i18n("Quit"), this);
	connect(action_quit, SIGNAL(triggered(bool)), SLOT(close()));
	toolbar->addAction(action_quit);

	toolbar->addSeparator();

	QPixmap icon_launch = KIconLoader::global()->loadIcon("start-here", KIconLoader::Small);
	QAction *action_launch = new QAction(QIcon(icon_launch), i18n("Launch a new game"), this);
	connect(action_launch, SIGNAL(triggered(bool)), SLOT(slotLaunch()));
	action_launch->setEnabled(false);
	toolbar->addAction(action_launch);

	setWindowTitle(i18n("Vencedor"));
	resize(800, 700);
	show();
}

void Vencedor::slotAbout()
{
	const KComponentData component = KGlobal::mainComponent();
	const KAboutData *about = component.aboutData();
	KAboutApplicationDialog dlg(about, this);
	dlg.exec();
}

void Vencedor::slotConnect()
{
	ConnectionDialog dlg(this);
	dlg.exec();
}

void Vencedor::slotLaunch()
{
}

void Vencedor::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	//KGGZCore::Room *room;

	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			break;
		case KGGZCore::CoreClient::negotiation:
			break;
		case KGGZCore::CoreClient::login:
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
			break;
		case KGGZCore::CoreClient::chat:
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
			m_core->initiateRoomChange(m_core->roomnames().at(0));
			for(int i = 0; i < m_core->roomnames().count(); i++)
			{
				Room *room = new Room(m_core->roomnames().at(i));
				room->setPlayers(0);
				m_rooms->addRoom(room);
			}
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

