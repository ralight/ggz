// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

/** @brief Spectator seat
 *
 * This class implements a seat which is always occupied by a spectator,
 * which doesn't participate actively in the game but still might need
 * information updates from time to time. Spectators might also choose
 * to become players, in these cases the spectator seat will be dissolved
 * and one of the player seats will be filled.
 */
public class Spectator implements Seat
{
	private String name;
	private int num;
	private GGZChannel client;

	/** @brief Constructor
	 *
	 * @param name Name of the spectator
	 * @param num Position in the list of spectator seats
	 * @param client Connection to the game client
	 *
	 * @internal
	 */
	public Spectator(String name, int num, GGZChannel client)
	{
		this.name = name;
		this.num = num;
		this.client = client;
	}

	/** @brief Return the name of the seat occupant
	 *
	 * Returns the name of the spectator who occupies the seat.
	 *
	 * @return Name of the seat occupant
	 */
	public String getName()
	{
		return this.name;
	}

	/** @brief Return the seat position
	 *
	 * Spectator seats on a table are ordered, starting from zero.
	 *
	 * @return Number of the seat in the seat list
	 */
	public int getNum()
	{
		return this.num;
	}

	/** @brief Return the communication channel
	 *
	 * @return Channel to the game client
	 */
	public GGZChannel getClient()
	{
		return this.client;
	}
}

