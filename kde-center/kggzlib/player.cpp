#include "player.h"

Player::Player(QString name)
{
	m_name = name;
	m_lag = -1;
	m_relation = Unknown;
	m_role = Guest;
}

QString Player::name()
{
	return m_name;
}

void Player::setStatistics(QString statistics)
{
	m_statistics = statistics;
}

void Player::setLag(int lag)
{
	m_lag = lag;
}

void Player::setRelation(Relation relation)
{
	m_relation = relation;
}

void Player::setRole(Role role)
{
	m_role = role;
}

QString Player::statistics()
{
	return m_statistics;
}

int Player::lag()
{
	return m_lag;
}

Player::Relation Player::relation()
{
	return m_relation;
}

Player::Role Player::role()
{
	return m_role;
}
