#include <kdebug.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <kprocess.h>
#include <kconfig.h>

#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qregexp.h>

#include "kopetemessage.h"
#include "kopetemessagemanagerfactory.h"
#include "kopetecontact.h"
#include "kopetemetacontact.h"

#include "ggz.h"

K_EXPORT_COMPONENT_FACTORY(kopete_ggz, KGenericFactory<GGZPlugin>("kopete_ggz"))

GGZPlugin::GGZPlugin(QObject *parent, const char *name, const QStringList &/*args*/)
: Kopete::Plugin(KGlobal::instance(), parent, name)
{
	connect(Kopete::ChatSessionManager::self(),
		SIGNAL(chatSessionCreated(Kopete::ChatSession*)),
		this,
		SLOT(slotNewKMM(Kopete::ChatSession*)));
	connect(Kopete::ChatSessionManager::self(),
		SIGNAL(aboutToDisplay(Kopete::Message&)),
		this,
		SLOT(slotProcessDisplay(Kopete::Message&)));

	kdDebug() << "GGZ PLUGIN INITIALIZED" << endl;
}

GGZPlugin::~GGZPlugin()
{
}

void GGZPlugin::slotNewKMM(Kopete::ChatSession *KMM)
{
	m_self = KMM->myself()->contactId();

	new GGZPluginGui(KMM);
}

void GGZPlugin::slotProcessDisplay(Kopete::Message& msg)
{
	kdDebug() << "RECEIVED MESSAGE FROM: " << msg.from()->contactId() << endl;
	kdDebug() << "* I AM RECEIVER: " << m_self << endl;
	kdDebug() << "* RECEIVED SUBJECT: " << msg.subject() << endl;
	kdDebug() << "* RECEIVED BODY: " << msg.plainBody() << endl;

	if(msg.from()->contactId() != m_self)
	{
		QRegExp re("^jeu\ (\\S+)$");
		if(re.exactMatch(msg.plainBody()))
		{
			GGZPluginLauncher *l = new GGZPluginLauncher(this);
			QString game = re.cap(1);
			l->launch("ggz-kop-peer", msg.from()->contactId(), game);
		}
	}
}

GGZPluginGui::GGZPluginGui(Kopete::ChatSession *parent, const char *name)
: QObject(parent, name), KXMLGUIClient(parent)
{
	(void)new KAction(i18n("Play GGZ game"), "ggz", 0, this, SLOT(slotGGZSelect()), actionCollection(), "playggz");
	setXMLFile("ggzchatui.rc");
	kdDebug() << "GGZ PLUGIN ACTIVATED IN MESSAGE WINDOW" << endl;

	session = parent;
}

void GGZPluginGui::slotGGZSelect()
{
	GGZPluginSelect *select;
	
	select = new GGZPluginSelect();
	connect(select, SIGNAL(signalSelected(QString)), SLOT(slotGGZ(QString)));
}

void GGZPluginGui::slotGGZ(QString game)
{
	kdDebug() << "PLAY GGZ GAME " << game << endl;

	Kopete::ContactPtrList players = session->members();
	for(Kopete::Contact *c = players.first(); c; c = players.next())
	{
		kdDebug() << "PLAY WITH " << c->contactId() << endl;
	}

	Kopete::Message msg(session->myself(), session->members(), QString::null, Kopete::Message::Outbound);
	msg.setBody(QString("%1 %2").arg("jeu").arg(game));
	session->sendMessage(msg);

	GGZPluginLauncher *l = new GGZPluginLauncher(this);
	l->launch(session->myself()->contactId(), QString::null, game);
}

GGZPluginLauncher::GGZPluginLauncher(QObject *parent)
: m_parent(parent)
{
}

void GGZPluginLauncher::launch(QString player1, QString player2, QString game)
{
	kdDebug() << "LAUNCHER " << player1 << " " << player2 << ": " << game << endl;

	KConfig conf("kopeteggzrc");
	conf.setGroup("default");

	QString server = conf.readEntry("Server", "live.ggzgamingzone.org");

	KProcess *proc = new KProcess(m_parent);
	*proc << "ggz-wrapper";
	*proc << "-s" << server;
	*proc << "-g" << game;
	*proc << "-u" << player1;
	if(!player2.isNull())
		*proc << "-d" << player2;
	proc->start(KProcess::DontCare);
}

GGZPluginSelect::GGZPluginSelect(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QPushButton *ok;

	m_gamescombo = new QComboBox(this);

	m_gamescombo->insertItem("TicTacToe");
	m_gamescombo->insertItem("Reversi");
	m_gamescombo->insertItem("Chess");

	ok = new QPushButton(i18n("Play"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_gamescombo);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotSelected()));

	show();
}

void GGZPluginSelect::slotSelected()
{
	emit signalSelected(m_gamescombo->currentText());
	close();
}

#include "ggz.moc"

// vim: set noet ts=4 sts=4 sw=4:

