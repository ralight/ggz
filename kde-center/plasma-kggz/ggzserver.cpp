#include "ggzserver.h"

GGZServer::GGZServer()
{
}

GGZServer::~GGZServer()
{
}

void GGZServer::setUri(const QString &uri)
{
	m_uri = uri;
}

QString GGZServer::uri() const
{
	return m_uri;
}

void GGZServer::setApi(const QString &api)
{
	m_api = api;
}

QString GGZServer::api() const
{
	return m_api;
}
