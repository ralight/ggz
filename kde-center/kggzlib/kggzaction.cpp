#include <kaction.h>
#include <kicon.h>
#include <klocale.h>
#include <kactioncollection.h>

#include "kggzaction.h"

KAction* kggzAction(const QString& what,
	const QObject *recvr, const char *slot, KActionCollection* parent,
	const char *name)
{
    QString data = what;
    if (data.isEmpty()) {
        data = i18n("Connect to GGZ Gaming Zone");
    }
    KAction *action = new KAction(data, parent);
    parent->addAction(name, action);
    action->setIcon(KIcon("ggz"));
    QObject::connect(action, SIGNAL(triggered(bool)), recvr, slot);

    return action;
}

