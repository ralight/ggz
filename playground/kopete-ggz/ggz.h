#ifndef GGZPLUGIN_H
#define GGZPLUGIN_H

#include <kxmlguiclient.h>

#include <qobject.h>
#include <qmap.h>

#include "kopetemessage.h"
#include "kopeteplugin.h"

class QStringList;
namespace Kopete { class Message; }
namespace Kopete { class MetaContact; }

class GGZPluginGui : public QObject, public KXMLGUIClient
{
	Q_OBJECT
public:
	GGZPluginGui(Kopete::ChatSession *parent, const char *name = 0);
private slots:
	void slotGGZ();
};

class GGZPlugin : public Kopete::Plugin
{
	Q_OBJECT
public:
	GGZPlugin( QObject *parent, const char *name, const QStringList &args );
	~GGZPlugin();

	bool serialize( Kopete::MetaContact *metaContact,
			QStringList &strList) const;
	void deserialize( Kopete::MetaContact *metaContact, const QStringList& data );

public slots:
	void slotProcessDisplay( Kopete::Message& msg );
	void slotProcessSend( Kopete::Message& msg );

protected:
	void changeMessage( Kopete::Message& msg );

private slots:
	void slotNewKMM(Kopete::ChatSession *);

private:
	QMap<const Kopete::MetaContact*, Q_UINT32> mMsgCountMap;
};

#endif

