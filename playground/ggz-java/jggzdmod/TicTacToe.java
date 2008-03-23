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

	public void joinEvent(Seat seat)
	{
	}

	public void leaveEvent(Seat seat)
	{
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

