#include "room.h"

Room::Room(QString name)
{
	m_name = name;
	m_access = Open;
	m_module = false;
}

void Room::setLogo(QString logo)
{
	m_logo = logo;
}

void Room::setAccess(Access access)
{
	m_access = access;
}

void Room::setModule(bool module)
{
	m_module = module;
}

QString Room::name()
{
	return m_name;
}

QString Room::logo()
{
	return m_logo;
}

Room::Access Room::access()
{
	return m_access;
}

bool Room::module()
{
	return m_module;
}
