// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

import java.util.ArrayList;

/** @brief GGZ protocol handler
 *
 * This is the abstract base class for GGZDMod::GGZDMod, implementing most
 * of the methods needed to interact with the GGZ server.
 * ....
 */
public abstract class Handler extends Protocol
{
	private int state = STATE_CREATED;
	private ArrayList seats = new ArrayList();
	private ArrayList spectators = new ArrayList();
	private GGZChannel channel = null;
	private GGZChannelPoller poller = null;

	/** @brief Constructor
	 *
	 * Does nothing of interest.
	 */
	public Handler()
	{
	}

	/** @brief Connect to the GGZ server.
	 *
	 * This method is used within GGZDMod::GGZDMod upon instantiation
	 * and doesn't need to be called.
	 *
	 * @internal
	 */
	protected void connect(int fd)
	{
		this.channel = new GGZChannel(fd);
		this.poller = new GGZChannelPoller();
		this.poller.registerChannel(this.channel);
	}

	/** @brief Return the list of seats
	 *
	 * Returns all seats of the game server as a list of GGZDMod::Player
	 * objects. The seats might be open, occupied by players or by bots,
	 * or be of other types.
	 *
	 * @return List of GGZDMod::Player objects
	 */
	protected ArrayList getSeats()
	{
		return this.seats;
	}

	/** @brief Return the list of spectators
	 *
	 * Returns all spectator seats of the game server as a list of
	 * GGZDMod::Spectator objects.
	 *
	 * @return List of GGZDMod::Spectator objects
	 */
	protected ArrayList getSpectators()
	{
		return this.spectators;
	}

	/** @brief Return the current state of the game server
	 *
	 * The game server is in full control of its state and should
	 * react on state changes directly. Thus, this method rarely needs
	 * to be used.
	 *
	 * @return Current state according to GGZDMod::Protocol
	 *
	 * @see setState
	 */
	protected int getState()
	{
		return this.state;
	}

	/** @brief Request a state change of the game server
	 *
	 * Calling this method requests a state change which must be
	 * confirmed by the GGZ server first. If the state change is legal,
	 * a stateEvent will be sent afterwards to confirm the request.
	 *
	 * Usually a game will transition from WAITING to PLAYING if all
	 * players have joined, and fall back to WAITING if anyone leaves.
	 * Finally, if the game is finished, the state will be set to DONE.
	 * The game server will then quit unless its KeepAlive option is
	 * set in the .dsc file.
	 *
	 * @param state New state of the game server
	 *
	 * @see getState
	 */
	protected void setState(int state)
	{
		this.state = state;
		try
		{
			this.channel.writeInt(REQ_GAME_STATE);
			this.channel.writeByte((byte)state);
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Send a log message to the GGZ server
	 *
	 * Sending a message to GGZ might lead to it being logged into
	 * a file or printed to the console, depending on the configuration
	 * of the GGZ server. Each log entry will conveniently be prefixed
	 * with the game name.
	 *
	 * @param s Message to log
	 */
	protected void log(String s)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(MSG_LOG);
				this.channel.writeString(s);
			}
		}
		catch(Exception e)
		{
			// can't do anything here
		}
	}

	/** @brief Request to resize the table
	 *
	 * If a table is too small, and too few players can participate in
	 * a game, the game server can request it to become larger. Similarly,
	 * tables currently too large can shrink.
	 *
	 * @param seats Number of seats on the resized table
	 */
	void resizeTable(int seats)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(REQ_NUM_SEATS);
				this.channel.writeInt(seats);
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Boot a player from the table
	 *
	 * The player's game client will be disconnected.
	 *
	 * @param name Name of the player to be booted
	 */
	void bootPlayer(String name)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(REQ_BOOT);
				this.channel.writeString(name);
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Insert a bot into an open seat
	 *
	 * Useful for adding AI players for human players who have left
	 * the game.
	 *
	 * @param seatnum Number of an open seat which will receive a bot player
	 */
	void insertBot(int seatnum)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(REQ_BOT);
				this.channel.writeInt(seatnum);
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Remove a bot from a seat
	 *
	 * The seat will be opened up and can subsequently be occupied
	 * by a human player again.
	 *
	 * @param seatnum Number of a seat currently occupied by a bot player
	 */
	void removeBot(int seatnum)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(REQ_OPEN);
				this.channel.writeInt(seatnum);
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Report a game result
	 *
	 * This method returns the game outcome and the scores of all players
	 * to the GGZ server. For all players, GGZDMod::Player::setScore should be
	 * called before.
	 */
	void reportGame()
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(MSG_GAME_REPORT);
				int scoreplayers = 0;
				for(int i = 0; i < this.seats.size(); i++)
				{
					Player p = (Player)this.seats.get(i);
					if(p.getScore() != null)
						scoreplayers++;
				}
				this.channel.writeInt(scoreplayers);
				for(int i = 0; i < this.seats.size(); i++)
				{
					Player p = (Player)this.seats.get(i);
					Score score = (Score)p.getScore();
					if(score != null)
					{
						int team = score.getTeam();
						if(team == -1)
							team = i;

						this.channel.writeString(p.getName());
						this.channel.writeInt(p.getType());
						this.channel.writeInt(team);
						this.channel.writeInt(score.getResult());
						this.channel.writeInt(score.getScore());
					}
				}
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Report a savegame to the GGZ server
	 *
	 * Savegames are useful to track a game's progress and outcome, but also to let
	 * a game server continue an interrupted game in case of connection or server
	 * problems.
	 *
	 * @param filename Path name of a savegame file
	 */
	void reportSavegame(String filename)
	{
		try
		{
			if(this.channel != null)
			{
				this.channel.writeInt(MSG_SAVEGAME_REPORT);
				this.channel.writeString(filename);
			}
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	/** @brief Main loop for the game server
	 *
	 * Calling handle() is the last action a game server performs after
	 * initialisation. Any event afterwards will be delivered through the
	 * appropriate event handlers.
	 */
	protected void handle()
	throws Exception
	{
		// FIXME: we do not recognise a broken channel atm.
		// FIXME: we also need this.poller.unregisterChannel()
		GGZChannel active = this.poller.poll();

		if(active != this.channel)
		{
			Seat seat = null;
			for(int i = 0; i < this.seats.size(); i++)
			{
				Player s = (Player)this.seats.get(i);
				if((s != null) && (s.getClient() == active))
				{
					seat = s;
					break;
				}
			}
			for(int i = 0; i < this.spectators.size(); i++)
			{
				Spectator s = (Spectator)this.spectators.get(i);
				if((s != null) && (s.getClient() == active))
				{
					seat = s;
					break;
				}
			}
			if(seat != null)
				dataEvent(seat);
			return;
		}

		int op = this.channel.readInt();

		String name;
		int num;
		int type;
		int fd;
		GGZChannel client = null;

		switch(op)
		{
			case MSG_GAME_LAUNCH:
				String gamename = this.channel.readString();
				gamename = gamename + "";
				// FIXME: we don't need gamename here
				int seats = this.channel.readInt();
				int spectators = this.channel.readInt();
				for(num = 0; num < spectators; num++)
					this.spectators.add(null);
				for(num = 0; num < seats; num++)
				{
					type = this.channel.readInt();
					name = null;
					if((type == Player.TYPE_RESERVED)
					|| (type == Player.TYPE_BOT))
						name = this.channel.readString();

					Player player = new Player(name, num, null, type);
					this.seats.add(player);
				}
				setState(STATE_WAITING);
				break;
			case MSG_GAME_SEAT:
				num = this.channel.readInt();
				type = this.channel.readInt();
				name = this.channel.readString();
				fd = this.channel.readfd();

				if(fd != -1)
				{
					client = new GGZChannel(fd);
					this.poller.registerChannel(client);
				}
				Player player = new Player(name, num, client, type);
				//Player oldplayer = (Player)this.seats.get(num);
				//this.seats.ensureCapacity(num + 1);
				this.seats.set(num, player);
				if(player.getType() == Player.TYPE_OPEN)
					seatEvent(player, null);
				else
					seatEvent(null, player);
				break;
			case MSG_GAME_SPECTATOR_SEAT:
				num = this.channel.readInt();
				name = this.channel.readString();
				fd = this.channel.readfd();

				if(fd != -1)
				{
					client = new GGZChannel(fd);
					this.poller.registerChannel(client);
				}
				Spectator spectator = new Spectator(name, num, client);
				//Spectator oldspectator = (Spectator)this.spectators.get(num);
				//this.spectators.ensureCapacity(num + 1);
				this.spectators.set(num, spectator);
				if(spectator.getClient() == null)
					seatEvent(spectator, null);
				else
					seatEvent(null, spectator);
				break;
			case MSG_GAME_RESEAT:
				int was_spectator = this.channel.readInt();
				int oldnum = this.channel.readInt();
				int is_spectator = this.channel.readInt();
				int newnum = this.channel.readInt();

				Seat oldseat, newseat;
				if(was_spectator == 1)
					oldseat = (Spectator)this.spectators.get(oldnum);
				else
					oldseat = (Player)this.seats.get(oldnum);
				if(is_spectator == 1)
					newseat = (Spectator)this.spectators.get(newnum);
				else
					newseat = (Player)this.seats.get(newnum);
				seatEvent(oldseat, newseat);
				break;
			case RSP_GAME_STATE:
				stateEvent(this.state);
				break;
			case MSG_SAVEDGAMES:
				// FIXME (ggzdmod): there shouldn't ever be more than one here
				int count = this.channel.readInt();
				for(int i = 0; i < count; i++)
				{
					String savegame = this.channel.readString();
					savegameEvent(savegame);
				}
				break;
			default:
				throw new Exception("handler: invalid opcode");
		}
	}

	/** @brief Callback for GGZ table seat changes
	 *
	 * Players in GGZ are organized sitting around a table, with spectators standing
	 * beside them and watching, speaking figuratively. Whenever players stand up or
	 * spectators sit down, or either of them join or leave, this callback is invoked.
	 * The seat parameters can be casted to either GGZDMod::Player or GGZDMod::Spectator
	 * to get detailed information about the seat occupants.
	 * 
	 * @param oldseat Seat occupation before the event happened
	 * @param newseat Seat occupation after the event happened
	 */
	abstract protected void seatEvent(Seat oldseat, Seat newseat);

	/** @brief Callback for game server state changes
	 *
	 * This event delivers the old state as a parameter. The new one can be queried in
	 * GGZDMod::Handler::getState.
	 *
	 * @param state Old state of the game server
	 */
	abstract protected void stateEvent(int state);

	/** @brief Callback for loaded savegames
	 *
	 * This event, which is fired shortly after a game starts, tells the game server
	 * that the game is being continued from a previous one, and that the game should
	 * be initialized from the savegame. The game's state will be STATE_SAVEGAME and
	 * should transition to STATE_WAITING afterwards.
	 *
	 * @param savegame Path to the savegame file
	 */
	abstract protected void savegameEvent(String savegame);

	/** @brief Call for data from a client
	 *
	 * In this event, a player or spectator sent some data which must be processed
	 * by the game server. To do so, GGZDMod::Seat::getClient must be called and its
	 * operations such like GGZDMod::GGZChannel::getInt and so on will be available.
	 *
	 * @param seat The seat whose occupant sent the data
	 */
	abstract protected void dataEvent(Seat seat);
}

