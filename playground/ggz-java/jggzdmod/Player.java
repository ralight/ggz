// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

/** @brief Player class
 *
 * Objects of this class represents player seats, either occupied players
 * or bots or open seats.
 */
public class Player extends Spectator implements Seat
{
	/**< Symbolic value for an open seat */
	public final static int TYPE_OPEN = 1;
	/**< Symbolic value for a seat occupied by an AI player */
	public final static int TYPE_BOT = 2;
	/**< Symbolic value for a human player in a seat */
	public final static int TYPE_PLAYER = 3;
	/**< Symbolic value for a seat reserved for a specific player */
	public final static int TYPE_RESERVED = 4;
	/**< Symbolic value for a seat which has been abandoned by a player */
	public final static int TYPE_ABANDONED = 5;

	private int type;

	/** @brief Constructor
	 *
	 * Player objects are created by jGGZDMod, the application doesn't
	 * need to invoke the constructor.
	 *
	 * @param name Name of the seat occupant, which is set for players and bots
	 * @param num Position in the seat list, ranging from 0 to n-1
	 * @param client Communication channel to the game client
	 * @param type Type of the seat occupant
	 *
	 * @internal
	 */
	public Player(String name, int num, GGZChannel client, int type)
	{
		super(name, num, client);
		this.type = type;
	}

	/** @brief Return the type of this seat
	 *
	 * A seat can be of type TYPE_PLAYER or TYPE_BOT when active.
	 * Otherwise, it can be of type TYPE_OPEN, TYPE_RESERVED and TYPE_ABANDONED.
	 *
	 * @return Type of the seat
	 */
	public int getType()
	{
		return this.type;
	}
}

