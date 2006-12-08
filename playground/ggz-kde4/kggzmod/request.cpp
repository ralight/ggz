#include "request.h"

using namespace KGGZMod;

Request::Request(Type type)
{
	m_type = type;
}

Request::Type Request::type() const
{
	return m_type;
}

StateRequest::StateRequest(/*Module::State*/int state)
: Request(Request::state)
{
	data["state"] = QString::number(state);
}

StandRequest::StandRequest()
: Request(Request::stand)
{
}

SitRequest::SitRequest(int seat)
: Request(Request::sit)
{
	data["seat"] = QString::number(seat);
}

BootRequest::BootRequest(QString playername)
: Request(Request::boot)
{
	data["player"] = playername;
}

BotRequest::BotRequest(int seat)
: Request(Request::bot)
{
	data["seat"] = QString::number(seat);
}

OpenRequest::OpenRequest(int seat)
: Request(Request::open)
{
	data["seat"] = QString::number(seat);
}

ChatRequest::ChatRequest(QString message)
: Request(Request::chat)
{
	data["message"] = message;
}

InfoRequest::InfoRequest(int seat)
: Request(Request::info)
{
	data["seat"] = QString::number(seat);
}

InfoRequest::InfoRequest()
: Request(Request::info)
{
	data["seat"] = -1;
}

