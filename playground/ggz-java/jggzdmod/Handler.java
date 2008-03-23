// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

import java.util.ArrayList;

public abstract class Handler extends Protocol
{
	private int state = STATE_CREATED;
	private ArrayList seats = new ArrayList();
	private ArrayList spectators = new ArrayList();
	private GGZChannel channel = null;
	private GGZChannelPoller poller = null;

	public Handler()
	{
	}

	protected void connect(int fd)
	{
		this.channel = new GGZChannel(fd);
		this.poller = new GGZChannelPoller();
		this.poller.registerChannel(this.channel);
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
				this.state = STATE_WAITING;
				stateEvent(this.state);
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

	abstract protected void seatEvent(Seat oldseat, Seat newseat);
	abstract protected void stateEvent(int state);
	abstract protected void savegameEvent(String savegame);
	abstract protected void dataEvent(Seat seat);
}

