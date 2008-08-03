#include "room.h"

Room::Room(QString name)
{
	m_name = name;
	m_access = Open;
	m_module = false;
	m_favourite = false;
	m_players = 0;
}

void Room::setDescription(QString description)
{
	m_description = description;
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

void Room::setFavourite(bool favourite)
{
	m_favourite = favourite;
}

void Room::setPlayers(int players)
{
	m_players = players;
}

QString Room::name()
{
	return m_name;
}

QString Room::description()
{
	return m_description;
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

bool Room::favourite()
{
	return m_favourite;
}

int Room::players()
{
	return m_players;
}
