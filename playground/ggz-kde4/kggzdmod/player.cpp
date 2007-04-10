#include "player.h"
#include "player_private.h"

using namespace KGGZdMod;

Player::Player()
{
	d = new PlayerPrivate();

	d->m_type = Player::unknown;
	d->m_seat = -1;
	d->m_fd = -1;
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

int Player::fd() const
{
	return d->m_fd;
}

void Player::init(PlayerPrivate *x)
{
	delete d;
	d = x;
}

