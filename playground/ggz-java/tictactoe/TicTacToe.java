// GGZ TicTacToe Java Server
// Java implementation of the classic TicTacToe server
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions

// Java Namespaces
import java.io.*;
import java.util.*;
import java.net.*;
import GGZ.GGZDMod;
import GGZ.GGZDModSeat;

// GGZDMod object
class Server extends GGZDMod
{
	public Server(String gamename)
	{
		super(gamename);
	}

	public void joinEvent(GGZDModSeat seat)
	{
	}

	public void leaveEvent(GGZDModSeat seat)
	{
	}

	public void dataEvent(byte[] data)
	{
	}
}

// Main game server class
class TicTacToe
{
	// Server entry point
	public static void main(String[] args)
	{
		Server server = new Server("tictactoe");
		server.connect();
		server.loop();
	}
}

