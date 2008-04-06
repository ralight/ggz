#include "kryds.h"

#include <kggzdmod/module.h>
#include <kggzdmod/player.h>

#include <kdebug.h>

Kryds::Kryds(QObject *parent)
: QObject(parent)
{
	m_module = new KGGZdMod::Module("Kryds");

	connect(m_module,
		SIGNAL(signalEvent(const KGGZdMod::Event&)),
		SLOT(slotEvent(const KGGZdMod::Event&)));

	m_module->loop();
}

void Kryds::slotEvent(const KGGZdMod::Event& event)
{
	kDebug() << "GGZ event:" << event.type();

	if(event.type() == KGGZdMod::Event::state)
	{
		KGGZdMod::StateEvent se(event);
		kDebug() << "State change: from" << se.oldstate() << "to" << se.state();
	}
	else if(event.type() == KGGZdMod::Event::playerseat)
	{
		KGGZdMod::PlayerSeatEvent pse(event);
		kDebug() << "Player seat change at #" << pse.seat().seat() << ": from"
			<< pse.oldseat().name() << "to" << pse.seat().name() <<
			"(seat change type " << pse.change() << ")";
	}
}
