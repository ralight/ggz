// GGZ Tic-Tac-Toe server based on the Java wrapper for GGZDMod
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

//import java.io.*;
//import java.util.*;
//import java.net.*;
import GGZDMod.*;

class TTTServer extends GGZDMod
{
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

