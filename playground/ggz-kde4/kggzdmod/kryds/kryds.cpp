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

		int open = 0;
		QList<KGGZdMod::Player*> players = m_module->players();
		for(int i = 0; i < players.size(); i++)
		{
			kDebug() << "* player at #" << i << "is" << players.at(i)->type();
			if(players.at(i)->type() == KGGZdMod::Player::open)
				open++;
		}

		if(!open)
		{
			kDebug() << "All seats full, start/continue game";
		}
		else
		{
			kDebug() << "Not all seats full, delay/interrupt game";
		}
	}
	else if(event.type() == KGGZdMod::Event::spectatorseat)
	{
		//KGGZdMod::SpectatorSeatEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::playerdata)
	{
		//KGGZdMod::PlayerDataEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::spectatordata)
	{
		//KGGZdMod::SpectatorDataEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::savegame)
	{
		//KGGZdMod::SavegameEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::error)
	{
		//KGGZdMod::ErrorEvent e(event);
		// ...
	}
}
