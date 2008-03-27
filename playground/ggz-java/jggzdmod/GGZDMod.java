// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

/** @mainpage
 *
 * jGGZDMod is a library of the <a href="http://www.ggzgamingzone.org/">GGZ Gaming Zone</a>
 * project which implements the <a href="http://www.ggzgamingzone.org/docs/spec/serverspec/">protocol</a>
 * between the main GGZ server and the game server.
 * It sends signals about player changes, arriving data, savegame continuations
 * and other interesting events to the game server, and can be used to query
 * information about the game state and to request changes in the game table's
 * configuration.
 *
 * Using GGZDMod::GGZDMod (which is also the main class for this package) starts by
 * inheriting from this class and reimplementing the four abstract methods provided
 * by its base class GGZDMod::Handler. Upon construction of your object, don't forget
 * to call super() to initialise the connection to GGZ. That's all!
 *
 * A typical reimplementation would do the following:
 * - In GGZDMod::Handler::stateEvent, react on state changes if needed.
 * - In GGZDMod::Handler::savegameEvent, load a savegame and continue to use it,
 *   instead of starting a new one. In particular, the game's internal state should
 *   be initialised from the contents of the savegame file. This is an advanced
 *   feature only needed if the game supports savegames.
 * - In GGZDMod::Handler::dataEvent, get the connection channel of the player
 *   whose connection contains the data by calling GGZDMod::Spectator::getClient.
 *   Use this channel to read and write data to the game client.
 * - In GGZDMod::Handler::seatEvent, check if the old seat is \b null for player
 *   joins, if the new seat is \b null for player leaves, or if both seats are
 *   set for seat switches, e.g. a player becoming a spectator or the other way
 *   around. In particular, this event can be used to find out if all seats are
 *   occupied by calling GGZDMod::Handler::getSeats and starting the game by
 *   changing its state with GGZDMod::Handler::setState(GGZDMod::Protocol::STATE_PLAYING).
 *
 *  A game server can always send log messages to the GGZ server by calling
 *  GGZDMod::GGZDMod::log.
 */

package GGZDMod;

/** @brief Main class of jGGZDMod
 *
 * Game servers inherit from this class to implement all the server-side
 * GGZ functionality.
 */
public abstract class GGZDMod extends Handler
{
	/** @brief Constructor
	 *
	 * This constructor sets up the game. It gets called by the game server
	 * class which inherits this class in its constructor through super().
	 *
	 * @param gamename Name of the game server
	 */
	public GGZDMod(String gamename)
	throws Exception
	{
		log("create game " + gamename);
		if(System.getenv("GGZMODE") == null)
		{
			throw new Exception("Game server not in GGZ mode.");
		}
		connect(new Integer(System.getenv("GGZSOCKET")).intValue());
	}

	/** @brief Logging method
	 *
	 * Logs a message both to GGZ and to the command line. Reimplement and use
	 * GGZDMod::Handler::log directly to override this behaviour.
	 *
	 * @param message Message to log to GGZ
	 *
	 * @see GGZDMod::Handler::log
	 */
	public void log(String message)
	{
		super.log(message);
		System.out.println("ggzdmod> " + message);
	}

	/** @brief Main loop
	 *
	 * This is the main loop for this library. It is a thin wrapper around
	 * GGZDMod::Handler::handle which catches exception. Reimplement in order
	 * to gain a more flexible exception handling.
	 */
	public void loop()
	{
		log("enter loop");
		while(true)
		{
			try
			{
				handle();
			}
			catch(Exception e)
			{
				log(e.toString());
				break;
			}
		}
	}

	/** @brief Return name of a game server state
	 *
	 * Utility method to return the human-readable string representation
	 * of the state of the game server.
	 *
	 * @param state State value, from GGZDMod::Protocol
	 *
	 * @return State name as a string
	 */
	public String stateName(int state)
	{
		switch(state)
		{
			case STATE_CREATED:
				return "created";
			case STATE_WAITING:
				return "waiting";
			case STATE_PLAYING:
				return "playing";
			case STATE_DONE:
				return "done";
			case STATE_RESTORED:
				return "restored";
		}

		return null;
	}
}

