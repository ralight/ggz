#include "kryds.h"

#include <kggzdmod/module.h>

Kryds::Kryds(QObject *parent)
: QObject(parent)
{
	m_module = new KGGZdMod::Module("Kryds");

	connect(m_module,
		SIGNAL(signalEvent(const KGGZdMod::Event&)),
		SLOT(slotEvent(const KGGZdMod::Event&)));
}

void Kryds::slotEvent(const KGGZdMod::Event& event)
{
	qDebug("event! %i", event.type());
}
