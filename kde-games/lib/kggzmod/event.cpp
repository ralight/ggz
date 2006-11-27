#include "event.h"

using namespace KGGZMod;

Event::Event(Type type)
{
	m_type = type;
}

Event::Type Event::type() const
{
	return m_type;
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
}

Player *SelfEvent::self() const
{
	QString p = data["player"];
	// FIXME: find out player - how to access list?
	return NULL;
}

SeatEvent::SeatEvent(const Event& event) : Event(Event::seat)
{
	data = event.data;
}

Player *SeatEvent::player() const
{
	QString p = data["player"];
	// FIXME: find out player - how to access list?
	return NULL;
}

ChatEvent::ChatEvent(const Event& event) : Event(Event::chat)
{
	data = event.data;
}

Player *ChatEvent::player() const
{
	QString p = data["player"];
	// FIXME: find out player - how to access list?
	return NULL;
}

QString ChatEvent::message() const
{
	return data["message"];
}

StatsEvent::StatsEvent(const Event& event) : Event(Event::stats)
{
	data = event.data;
}

InfoEvent::InfoEvent(const Event& event) : Event(Event::info)
{
	data = event.data;
}

Player *InfoEvent::player() const
{
	QString p = data["player"];
	// FIXME: find out player - how to access list?
	return NULL;
}

