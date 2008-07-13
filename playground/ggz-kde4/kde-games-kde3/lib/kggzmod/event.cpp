#include "event.h"

using namespace KGGZMod;

Event::Event(Type type)
{
	m_type = type;
	m_player = 0;
}

Event::Type Event::type() const
{
	return m_type;
}

Player *Event::player() const
{
	return m_player;
}

LaunchEvent::LaunchEvent(const Event& event) : Event(Event::launch)
{
	data = event.data;
}

ServerEvent::ServerEvent(const Event& event) : Event(Event::server)
{
	data = event.data;
}

int ServerEvent::fd() const
{
	QString f = data["fd"];
	return f.toInt();
}

SelfEvent::SelfEvent(const Event& event) : Event(Event::self)
{
	data = event.data;
	m_player = event.player();
}

Player *SelfEvent::self() const
{
	return m_player;
}

SeatEvent::SeatEvent(const Event& event) : Event(Event::seat)
{
	data = event.data;
	m_player = event.player();
}

Player *SeatEvent::player() const
{
	return m_player;
}

ChatEvent::ChatEvent(const Event& event) : Event(Event::chat)
{
	data = event.data;
	m_player = event.player();
}

Player *ChatEvent::player() const
{
	return m_player;
}

QString ChatEvent::message() const
{
	return data["message"];
}

StatsEvent::StatsEvent(const Event& event) : Event(Event::stats)
{
	data = event.data;
	m_player = event.player();
}

Player *StatsEvent::player() const
{
	return m_player;
}

InfoEvent::InfoEvent(const Event& event) : Event(Event::info)
{
	data = event.data;
	m_player = event.player();
}

Player *InfoEvent::player() const
{
	return m_player;
}

