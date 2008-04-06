#ifndef KGGZDMOD_EVENT_PRIVATE_H
#define KGGZDMOD_EVENT_PRIVATE_H

#include <kggzdmod/player.h>
#include <kggzdmod/player_private.h>

namespace KGGZdMod
{

class EventPrivate
{
	public:
		EventPrivate(){}
		QString m_message;
		Module::State m_state;
		Module::State m_oldstate;
		PlayerPrivate m_seat;
		PlayerPrivate m_oldseat;
		PlayerPrivate m_spectatorseat;
		PlayerPrivate m_oldspectatorseat;
		QString m_savegame;
};

}

#endif

