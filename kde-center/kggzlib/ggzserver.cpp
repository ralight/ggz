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

void GGZServer::setName(const QString &name)
{
	m_name = name;
}

QString GGZServer::name() const
{
	return m_name;
}

void GGZServer::setIcon(const QString &icon)
{
	m_icon = icon;
}

QString GGZServer::icon() const
{
	return m_icon;
}

void GGZServer::setLoginType(const QString &logintype)
{
	m_logintype = logintype;
}

QString GGZServer::loginType() const
{
	return m_logintype;
}
