// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

import java.util.ArrayList;
import java.nio.ByteBuffer;

class Handler extends Protocol
{
	private int state = STATE_CREATED;
	private int fd = -1;
	private ArrayList seats = new ArrayList();
	private ArrayList spectators = new ArrayList();
	private GGZChannel channel = null;

	public Handler()
	{
	}

	protected void connect(int fd)
	{
		this.fd = fd;
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
System.err.println("OP: " + op);

		switch(op)
		{
			case MSG_GAME_LAUNCH:
				String gamename = readString();
				int seats = readInt();
				int spectators = readInt();
				for(int i = 0; i < seats; i++)
				{
					int type = readInt();
					String name = null;
					if((type == Player.TYPE_RESERVED)
					|| (type == Player.TYPE_BOT))
						name = readString();
				}
System.err.println("LAUNCH: " + seats + " seats");
				this.state = STATE_WAITING;
				break;
			case MSG_GAME_SEAT:
				int num = readInt();
				int type = readInt();
				String name = readString();
				int fd = this.channel.readfd();
				GGZChannel client = new GGZChannel(fd);
System.err.println("PLAYER: " + num);
				Player player = new Player(name, num, fd, type);
				this.seats.ensureCapacity(num + 1);
				this.seats.add(num, player);
				break;
			case MSG_GAME_SPECTATOR_SEAT:
				/*int*/ num = readInt();
				/*String*/ name = readString();
				/*int*/ fd = this.channel.readfd();
				/*GGZChannel*/ client = new GGZChannel(fd);
System.err.println("SPECTATOR: " + num);
				Spectator spectator = new Spectator(name, num, fd);
				this.spectators.ensureCapacity(num + 1);
				this.spectators.add(num, spectator);
				break;
			case MSG_GAME_RESEAT:
				int was_spectator = readInt();
				int old_seat = readInt();
				int is_spectator = readInt();
				int new_seat = readInt();
				break;
			case RSP_GAME_STATE:
				break;
			case MSG_SAVEDGAMES:
				int count = readInt();
				for(int i = 0; i < count; i++)
				{
					String savegame = readString();
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
}

