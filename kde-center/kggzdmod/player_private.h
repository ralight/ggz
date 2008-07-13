#ifndef KGGZDMOD_PLAYER_PRIVATE_H
#define KGGZDMOD_PLAYER_PRIVATE_H

namespace KGGZdMod
{

class PlayerPrivate
{
	public:
		PlayerPrivate(){}
		Player::Type m_type;
		QString m_name;
		int m_seat;
		int m_fd;
};

}

#endif

