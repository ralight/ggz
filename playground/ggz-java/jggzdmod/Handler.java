// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

import java.util.ArrayList;
import java.nio.ByteBuffer;

public abstract class Handler extends Protocol
{
	private int state = STATE_CREATED;
	private ArrayList seats = new ArrayList();
	private ArrayList spectators = new ArrayList();
	private GGZChannel channel = null;

	public Handler()
	{
	}

	protected void connect(int fd)
	{
		this.channel = new GGZChannel(fd);
	}

	protected ArrayList getSeats()
	{
		return this.seats;
	}

	protected ArrayList getSpectators()
	{
		return this.spectators;
	}

	protected int getState()
	{
		return this.state;
	}

	protected void handle()
	throws Exception
	{
		// FIXME: we block on reading at the moment
		int op = readInt();

		String name;
		int num;
		int type;
		int fd;
		GGZChannel client = null;

		switch(op)
		{
			case MSG_GAME_LAUNCH:
				String gamename = readString();
				gamename = gamename + "";
				// FIXME: we don't need gamename here
				int seats = readInt();
				int spectators = readInt();
				for(num = 0; num < spectators; num++)
					this.spectators.add(null);
				for(num = 0; num < seats; num++)
				{
					type = readInt();
					name = null;
					if((type == Player.TYPE_RESERVED)
					|| (type == Player.TYPE_BOT))
						name = readString();

					Player player = new Player(name, num, null, type);
					this.seats.add(player);
				}
				this.state = STATE_WAITING;
				stateEvent(this.state);
				break;
			case MSG_GAME_SEAT:
				num = readInt();
				type = readInt();
				name = readString();
				fd = this.channel.readfd();

				if(fd != -1)
					client = new GGZChannel(fd);
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
				num = readInt();
				name = readString();
				fd = this.channel.readfd();

				if(fd != -1)
					client = new GGZChannel(fd);
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
				int was_spectator = readInt();
				int oldnum = readInt();
				int is_spectator = readInt();
				int newnum = readInt();

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
				int count = readInt();
				for(int i = 0; i < count; i++)
				{
					String savegame = readString();
					savegameEvent(savegame);
				}
				break;
			default:
				throw new Exception("handler: invalid opcode");
		}
	}

	private int readInt()
	throws Exception
	{
		ByteBuffer bb = ByteBuffer.allocate(4);
		int ret = this.channel.read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");

		byte[] data = bb.array();
		int op = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
		return op;
	}

	private String readString()
	throws Exception
	{
		int length = readInt();

		ByteBuffer bb = ByteBuffer.allocate(length);
		int ret = this.channel.read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");

		byte[] data = bb.array();
		return new String(data);
	}

	abstract protected void seatEvent(Seat oldseat, Seat newseat);
	abstract protected void stateEvent(int state);
	abstract protected void savegameEvent(String savegame);
}

