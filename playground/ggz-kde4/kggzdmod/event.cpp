#include "event.h"
#include "event_private.h"

using namespace KGGZdMod;

Event::Event(Type type)
{
	d = new EventPrivate();
	// FIXME: deletion in destructor? (same goes for Player)

	m_type = type;
}

Event::Type Event::type() const
{
	return m_type;
}

void Event::init(EventPrivate *x)
{
}

StateEvent::StateEvent(const Event& event) : Event(Event::state)
{
	//data = event.data;
	d = event.d;
}

Module::State StateEvent::state() const
{
	return d->m_state;
}

Module::State StateEvent::oldstate() const
{
	return d->m_oldstate;
}

PlayerSeatEvent::PlayerSeatEvent(const Event& event) : Event(Event::playerseat)
{
	//data = event.data;
	d = event.d;
}

PlayerSeat PlayerSeatEvent::seat() const
{
	Player p;
	p.init(&d->m_seat);
	return p;
}

PlayerSeat PlayerSeatEvent::oldseat() const
{
	Player p;
	p.init(&d->m_oldseat);
	return p;
}

PlayerSeatEvent::PlayerSeatChange PlayerSeatEvent::change() const
{
	PlayerPrivate oldseat = d->m_oldseat;
	PlayerPrivate seat = d->m_seat;

	if((oldseat.m_type != Player::player) && (seat.m_type == Player::player))
		return sitdown;
	if((oldseat.m_type == Player::player) && (seat.m_type != Player::player))
		return standup;

	if((oldseat.m_type != Player::bot) && (seat.m_type == Player::bot))
		return botsitdown;
	if((oldseat.m_type == Player::bot) && (seat.m_type != Player::bot))
		return botstandup;
	if((oldseat.m_type == Player::bot) && (seat.m_type == Player::bot))
		return botswap;

	if((oldseat.m_type != Player::reserved) && (seat.m_type == Player::reserved))
		return reservation;
	if((oldseat.m_type == Player::reserved) && (seat.m_type != Player::reserved))
		return dropreservation;

	// FIXME: "other" seat change events?
	return sitdown;
}

SpectatorSeatEvent::SpectatorSeatEvent(const Event& event) : Event(Event::spectatorseat)
{
	//data = event.data;
	d = event.d;
}

SpectatorSeat SpectatorSeatEvent::seat() const
{
	Player p;
	p.init(&d->m_spectatorseat);
	return p;
}

SpectatorSeat SpectatorSeatEvent::oldseat() const
{
	Player p;
	p.init(&d->m_oldspectatorseat);
	return p;
}

SpectatorSeatEvent::SpectatorSeatChange SpectatorSeatEvent::change() const
{
	PlayerPrivate oldseat = d->m_oldspectatorseat;
	PlayerPrivate seat = d->m_spectatorseat;

	if((oldseat.m_type != Player::spectator) && (seat.m_type == Player::spectator))
		return sitdown;
	if((oldseat.m_type == Player::spectator) && (seat.m_type != Player::spectator))
		return standup;

	// FIXME: "other" spectator seat change events?
	return sitdown;
}

PlayerDataEvent::PlayerDataEvent(const Event& event) : Event(Event::playerdata)
{
	//data = event.data;
	d = event.d;
}

SpectatorSeat PlayerDataEvent::seat() const
{
	Player p;
	p.init(&d->m_seat);
	return p;
}

SpectatorDataEvent::SpectatorDataEvent(const Event& event) : Event(Event::spectatordata)
{
	//data = event.data;
	d = event.d;
}

SpectatorSeat SpectatorDataEvent::seat() const
{
	Player p;
	p.init(&d->m_spectatorseat);
	return p;
}

ErrorEvent::ErrorEvent(const Event& event) : Event(Event::error)
{
	//data = event.data;
	d = event.d;
}

QString ErrorEvent::message() const
{
	//return data["message"];
	return d->m_message;
}

