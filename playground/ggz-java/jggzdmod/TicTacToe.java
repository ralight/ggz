// GGZ Tic-Tac-Toe server based on the Java wrapper for GGZDMod
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

import java.util.*;
import GGZDMod.*;

class TTTServer extends GGZDMod
{
	private final static int SNDMOVE = 0;
	private final static int REQSYNC = 1;
	private final static int MSGMOVE = 2;
	private final static int MSGGAMEOVER = 3;
	private final static int REQMOVE = 4;
	private final static int RSPMOVE = 5;
	private final static int SNDSYNC = 6;

	public TTTServer()
	throws Exception
	{
		super("Tic-Tac-Toe (java)");
	}

	protected void stateEvent(int state)
	{
		log("state: " + stateName(state));
	}

	protected void savegameEvent(String savegame)
	{
		log("savegame: " + savegame);
	}

	protected void dataEvent(Seat seat)
	{
		log("data: " + seat.getName());
	}

	protected void seatEvent(Seat oldseat, Seat newseat)
	{
		if(oldseat == null)
		{
			// player/spectator has joined
			if(newseat.getClass() == Player.class)
			{
				// it's a player
				log("join of player " + newseat.getName());
			}
			else
			{
				// it's a spectator
				log("join of spectator " + newseat.getName());
			}
		}
		else if(newseat == null)
		{
			// player/spectator has left
			log("leave of player or spectator " + oldseat.getName());
		}
		else
		{
			// seat change player<->spectator
			log("someone stood up, sat down or the like...");
		}

		ArrayList players = getSeats();
		Player p1 = (Player)players.get(0);
		Player p2 = (Player)players.get(1);
		if((p1.getType() != Player.TYPE_OPEN) & (p2.getType() != Player.TYPE_OPEN))
		{
			log("request move from player 0");

			try
			{
				p1.getClient().writeInt(REQMOVE);
			}
			catch(Exception e)
			{
				log("ERROR: " + e.toString());
			}
		}
	}
}

class TicTacToe
{
	public final static void main(String[] args)
	{
		try
		{
			TTTServer server = new TTTServer();
			server.loop();
		}
		catch(Exception e)
		{
			System.err.println("ERROR: " + e.toString());
		}
	}
}

