#include "request.h"

using namespace KGGZdMod;

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

LogRequest::LogRequest(QString message)
: Request(Request::log)
{
	data["message"] = message;
}

ResultRequest::ResultRequest()
: Request(Request::result)
{
}

SavegameRequest::SavegameRequest(QString filename)
: Request(Request::savegame)
{
	data["filename"] = filename;
}

