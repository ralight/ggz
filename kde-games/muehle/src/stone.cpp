#include "stone.h"

Stone::Stone()
{
	m_x = -1;
	m_y = -1;
	m_owner = -1;
}

Stone::~Stone()
{
}

void Stone::move(int x, int y)
{
	m_x = x;
	m_y = y;
}

void Stone::assign(int owner)
{
	m_owner = owner;
}

int Stone::x()
{
	return m_x;
}

int Stone::y()
{
	return m_y;
}

int Stone::owner()
{
	return m_owner;
}

