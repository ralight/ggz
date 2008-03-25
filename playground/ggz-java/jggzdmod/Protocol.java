// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

/** @brief Protocol message definitions
 *
 * This class defines the (internal) messages which can be sent between the
 * GGZ server and the game server. It also contains the possible game server
 * state values which are of interest to the game server.
 */
abstract class Protocol
{
	// internal: opcodes from game server to GGZ table
	protected final static int MSG_LOG = 0;
	protected final static int REQ_GAME_STATE = 1;
	protected final static int REQ_NUM_SEATS = 2;
	protected final static int REQ_BOOT = 3;
	protected final static int REQ_BOT = 4;
	protected final static int REQ_OPEN = 5;
	protected final static int MSG_GAME_REPORT = 6;
	protected final static int MSG_SAVEGAME_REPORT = 7;

	// internal: opcodes from GGZ table to game server
	protected final static int MSG_GAME_LAUNCH = 0;
	protected final static int MSG_GAME_SEAT = 1;
	protected final static int MSG_GAME_SPECTATOR_SEAT = 2;
	protected final static int MSG_GAME_RESEAT = 3;
	protected final static int RSP_GAME_STATE = 4;
	protected final static int MSG_SAVEDGAMES = 5;

	/**< The game server has been launched, but no information from GGZ is available yet. */
	public final static int STATE_CREATED = 0;
	/**< The game server has (through jGGZDMod) received all required startup information.
	 *   Alternatively, the game server is waiting for players again. */
	public final static int STATE_WAITING = 1;
	/**< The game server has decided to start the game. */
	public final static int STATE_PLAYING = 2;
	/**< The game server has finished the game. */
	public final static int STATE_DONE = 3;
	/**< A savegame is being restored. */
	public final static int STATE_RESTORED = 4;
}

