#include <kdebug.h>
#include <kgenericfactory.h>
//#include <kstandarddirs.h>
#include <kaction.h>
#include <kmainwindow.h>
#include <kmenubar.h>

#include <qpopupmenu.h>

#include "kopetemessage.h"
#include "kopetemessagemanagerfactory.h"
#include "kopetecontact.h"
#include "kopetemetacontact.h"

#include "ggz.h"

//#include <qcolor.h>

K_EXPORT_COMPONENT_FACTORY( kopete_ggz, KGenericFactory<GGZPlugin>( "kopete_ggz" )  )

GGZPlugin::GGZPlugin( QObject *parent, const char *name, const QStringList &/*args*/ )
: Kopete::Plugin( KGlobal::instance(), parent, name )
{
	connect(Kopete::ChatSessionManager::self(),
		SIGNAL(chatSessionCreated(Kopete::ChatSession*)),
		this,
		SLOT(slotNewKMM(Kopete::ChatSession*)));

	kdDebug() << "GGZ PLUGIN INITIALIZED" << endl;
}

GGZPlugin::~GGZPlugin()
{
}

GGZPluginGui::GGZPluginGui(Kopete::ChatSession *parent, const char *name)
: QObject(parent, name), KXMLGUIClient(parent)
{
	(void)new KAction(i18n("Play GGZ game"), "ggz", 0, this, SLOT(slotGGZ()), actionCollection(), "playggz");
	setXMLFile("ggzchatui.rc");
	kdDebug() << "GGZ PLUGIN ACTIVATED IN MESSAGE WINDOW" << endl;

	Kopete::ContactPtrList players = parent->members();
	for(Kopete::Contact *c = players.first(); c; c = players.next())
	{
		kdDebug() << "PLAY WITH" << c->contactId() << endl;
	}
}

void GGZPluginGui::slotGGZ()
{
	kdDebug() << "PLAY GGZ GAME" << endl;
}

void GGZPlugin::slotNewKMM( Kopete::ChatSession *KMM )
{
	new GGZPluginGui(KMM);
}

bool
GGZPlugin::serialize(Kopete::MetaContact *metaContact, QStringList &strList) const
{
Q_UNUSED(metaContact);
Q_UNUSED(strList);
	return true;
}

void
GGZPlugin::deserialize( Kopete::MetaContact *metaContact, const QStringList& data )
{
Q_UNUSED(metaContact);
Q_UNUSED(data);
}

void
GGZPlugin::slotProcessDisplay( Kopete::Message& msg )
{
Q_UNUSED(msg);
}

void
GGZPlugin::slotProcessSend( Kopete::Message& msg )
{
Q_UNUSED(msg);
}

void
GGZPlugin::changeMessage( Kopete::Message& msg )
{
Q_UNUSED(msg);
}

#include "ggz.moc"

// vim: set noet ts=4 sts=4 sw=4:

