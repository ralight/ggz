#ifndef KGGZMOD_PLAYER_H
#define KGGZMOD_PLAYER_H

#include <qstring.h>

namespace KGGZMod
{

class PlayerPrivate;
class Statistics;

class Player
{
	friend class ModulePrivate;

	public:
		enum Type
		{
			unknown,
			open,
			bot,
			player,
			reserved,
			abandoned,
			spectator
		};

		Type type() const;

		QString name() const;
		int seat() const;

		Statistics *stats() const;

		QString hostname() const;
		QString photo() const;
		QString realname() const;

	private:
		Player();
		PlayerPrivate *d;
		void init(PlayerPrivate *x);
};

}

#endif

