#include <kdebug.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <kprocess.h>

#include <qpopupmenu.h>

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
		if(msg.plainBody() == "jeu")
		{
			GGZPluginLauncher *l = new GGZPluginLauncher(this);
			l->launch("ggz-kop-peer", msg.from()->contactId());
		}
	}
}

GGZPluginGui::GGZPluginGui(Kopete::ChatSession *parent, const char *name)
: QObject(parent, name), KXMLGUIClient(parent)
{
	(void)new KAction(i18n("Play GGZ game"), "ggz", 0, this, SLOT(slotGGZ()), actionCollection(), "playggz");
	setXMLFile("ggzchatui.rc");
	kdDebug() << "GGZ PLUGIN ACTIVATED IN MESSAGE WINDOW" << endl;

	session = parent;
}

void GGZPluginGui::slotGGZ()
{
	kdDebug() << "PLAY GGZ GAME" << endl;

	Kopete::ContactPtrList players = session->members();
	for(Kopete::Contact *c = players.first(); c; c = players.next())
	{
		kdDebug() << "PLAY WITH " << c->contactId() << endl;
	}

	Kopete::Message msg(session->myself(), session->members(), "jeu", Kopete::Message::Outbound);
	//msg.setBody("jeu");
	session->sendMessage(msg);

	GGZPluginLauncher *l = new GGZPluginLauncher(this);
	l->launch(session->myself()->contactId(), QString::null);
}

GGZPluginLauncher::GGZPluginLauncher(QObject *parent)
: m_parent(parent)
{
}

void GGZPluginLauncher::launch(QString player1, QString player2)
{
	kdDebug() << "LAUNCHER " << player1 << " " << player2 << endl;

	KProcess *proc = new KProcess(m_parent);
	*proc << "ggz-wrapper";
	*proc << "-s" << "live.ggzgamingzone.org";
	*proc << "-g" << "TicTacToe";
	*proc << "-u" << player1;
	if(!player2.isNull())
		*proc << "-d" << player2;
	proc->start(KProcess::DontCare);
}

#include "ggz.moc"

// vim: set noet ts=4 sts=4 sw=4:

