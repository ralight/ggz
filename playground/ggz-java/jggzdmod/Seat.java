// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

/** @brief Seat on a GGZ table
 *
 * Objects implementing this interface represent player seats on a game table.
 * Each seat can be occupied by a human or a bot, or it can be empty. It could
 * also be reserved for a human or abandoned by a human player.
 */
public interface Seat
{
	/** @brief Return the name of the seat occupant
	 *
	 * Returns the name of whoever occupies the seat. This might be the
	 * empty string for open seats, or a player, spectator or bot name.
	 *
	 * @return Name of the seat occupant
	 */
	public String getName();

	/** @brief Return the seat position
	 *
	 * Seats on a table are ordered, starting from zero. Player and Spectator
	 * seats however are in different lists, therefore the number should only
	 * be used in combination of ensuring the player or spectator status of
	 * a seat.
	 *
	 * @return Number of the seat in the seat list
	 */
	public int getNum();
}

