#ifndef KGGZMOD_PLAYER_PRIVATE_H
#define KGGZMOD_PLAYER_PRIVATE_H

namespace KGGZMod
{

class PlayerPrivate
{
	public:
		PlayerPrivate(){}
		Player::Type m_type;
		QString m_name;
		int m_seat;
		Statistics *m_stats;
		QString m_hostname;
		QString m_photo;
		QString m_realname;
};

}

#endif

