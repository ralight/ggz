#include <kggzdmod/module.h>

KGGZdMod::Module *mod;

Test::Test()
{
	mod = new KGGZdMod();

	connect(mod,
		SIGNAL(signalEvent(KGGZdMod::Event)),
		SLOT(slotEvent(KGGZdMod::Event)));
}

Test::~Test()
{
	delete mod;
}

void Test::slotEvent(KGGZdMod::Event event)
{
	if(event.type() == KGGZdMod::Event::state)
	{
		KGGZdMod::StateEvent e(event);
		KGGZdMod::Module::State state = e.state();
		KGGZdMod::Module::State oldstate = e.oldstate();
		// ...
	}
	else if(event.type() == KGGZdMod::Event::playerseat)
	{
		KGGZdMod::PlayerSeatEvent e(event);
		KGGZdMod::PlayerSeat seat = e.seat();
		KGGZdMod::PlayerSeat oldseat = e.oldseat();
		KGGZdMod::PlayerSeatEvent::PlayerSeatChange change = e.change();
		// ...
		if(change == KGGZdMod::PlayerSeatEvent::sitdown)
		{
			// player join...
		}
	}
	else if(event.type() == KGGZdMod::Event::spectatorseat)
	{
		KGGZdMod::SpectatorSeatEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::playerdata)
	{
		KGGZdMod::PlayerDataEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::spectatordata)
	{
		KGGZdMod::SpectatorDataEvent e(event);
		// ...
	}
	else if(event.type() == KGGZdMod::Event::error)
	{
		KGGZdMod::ErrorEvent e(event);
		// ...
	}
}

