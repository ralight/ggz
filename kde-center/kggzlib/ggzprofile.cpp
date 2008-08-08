#include "ggzprofile.h"

#include "ggzserver.h"

GGZProfile::GGZProfile()
{
	m_logintype = unconfigured;
}

GGZProfile::~GGZProfile()
{
}

void GGZProfile::setGGZServer(const GGZServer &server)
{
	m_server = server;
}

GGZServer GGZProfile::ggzServer() const
{
	return m_server;
}

void GGZProfile::setLoginType(const LoginType logintype)
{
	m_logintype = logintype;
}

GGZProfile::LoginType GGZProfile::loginType() const
{
	return m_logintype;
}

void GGZProfile::setUsername(const QString &username)
{
	m_username = username;
}

QString GGZProfile::username() const
{
	return m_username;
}

void GGZProfile::setPassword(const QString &password)
{
	m_password = password;
}

QString GGZProfile::password() const
{
	return m_password;
}

void GGZProfile::setRoomname(const QString &roomname)
{
	m_roomname = roomname;
}

QString GGZProfile::roomname() const
{
	return m_roomname;
}

void GGZProfile::setRealname(const QString &realname)
{
	m_realname = realname;
}

QString GGZProfile::realname() const
{
	return m_realname;
}

void GGZProfile::setEmail(const QString &email)
{
	m_email = email;
}

QString GGZProfile::email() const
{
	return m_email;
}

void GGZProfile::assign(const GGZProfile profile)
{
	setLoginType(profile.loginType());
	setUsername(profile.username());
	setPassword(profile.password());
	setRoomname(profile.roomname());
	setRealname(profile.realname());
	setEmail(profile.email());
	setGGZServer(profile.ggzServer());
}

bool GGZProfile::configured()
{
	bool configured = true;
	if(m_logintype == unconfigured)
		configured = false;
	if(m_username.isEmpty())
		configured = false;
	return configured;
}
