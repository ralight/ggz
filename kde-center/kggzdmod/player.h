#ifndef KGGZDMOD_PLAYER_H
#define KGGZDMOD_PLAYER_H

#include <qstring.h>

namespace KGGZdMod
{

class PlayerPrivate;

/**
 * @short Player representation of an online game.
 *
 * Every participant in an online game is represented by an object
 * of this class. This includes active players, but also passive
 * spectators, bots and even open seats. The \ref Type attribute
 * differentiates those objects.
 *
 * Players who are connected over the network (like humans,
 * spectators and external AI players) will have a valid file
 * descriptor.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class Player
{
	friend class ModulePrivate;
	friend class PlayerSeatEvent;
	friend class SpectatorSeatEvent;
	friend class PlayerDataEvent;
	friend class SpectatorDataEvent;

	public:
		/**
		 * The type of which a seat can be.
		 * While spectators sit on the sidelines, players
		 * and bots along with open/reserved/abandoned seats
		 * are the ones which participate in a game.
		 */
		enum Type
		{
			unknown,	/**< The type of a seat is not yet known. */
			open,		/**< A seat is open and also not reserved. */
			bot,		/**< This is a bot player (a server-side AI player). */
			player,		/**< This is a regular player. */
			reserved,	/**< The seat is reserved for someone. */
			abandoned,	/**< The seat has been abandoned by a player. */
			spectator	/**< This is a spectator who does only watch the game. */
		};

		/**
		 * The type a seat has.
		 *
		 * @return seat type
		 */
		Type type() const;

		/**
		 * The name of a player.
		 *
		 * This is the actual name for normal players and for bot
		 * players. It refers to a name for reserved and abandoned
		 * seats. Open seats do not have a name associated with them.
		 *
		 * @return name of the player or bot
		 */
		QString name() const;

		/**
		 * The seat number.
		 *
		 * This number is used to associate a player object uniquely
		 * with other information.
		 *
		 * @return number of the seat
		 */
		int seat() const;

		/**
		 * File descriptor for a given player.
		 *
		 * Those players who are connected over the network will have a file
		 * descriptor from which one can read data.
		 *
		 * @return player file descriptor, or \b -1 if not present
		 */
		int fd() const;

	private:
		Player();
		PlayerPrivate *d;
		void init(PlayerPrivate *x);
};

}

#endif

