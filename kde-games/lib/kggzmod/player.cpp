#include "player.h"
#include "player_private.h"

using namespace KGGZMod;

Player::Player()
{
	d = new PlayerPrivate();

	d->m_type = Player::unknown;
	d->m_seat = -1;
}

Player::Type Player::type() const
{
	return d->m_type;
}

QString Player::name() const
{
	return d->m_name;
}

int Player::seat() const
{
	return d->m_seat;
}

Statistics *Player::stats() const
{
	return d->m_stats;
}

QString Player::hostname() const
{
	return d->m_hostname;
}

QString Player::photo() const
{
	return d->m_photo;
}

QString Player::realname() const
{
	return d->m_realname;
}

void Player::init(PlayerPrivate *x)
{
	delete d;
	d = x;
}

