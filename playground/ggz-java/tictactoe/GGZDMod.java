// GGZ TicTacToe Java Server
// Java implementation of the classic TicTacToe server
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions

// Package declaration
package GGZ;

// Java namespaces
import java.io.*;
import java.util.*;
import java.net.*;
import GGZ.GGZDModSeat;

// Game server protocol class
abstract class GGZDModProto
{
	protected final static int rsp_game_join = 0;
	protected final static int rsp_game_leave = 1;
	protected final static int msg_log = 2;
	protected final static int req_game_state = 3;
	protected final static int msg_game_launch = 0;
	protected final static int req_game_join = 1;
	protected final static int req_game_leave = 2;
	protected final static int rsp_game_state = 3;
}

// Game server handler class
class GGZDModHandler extends GGZDModProto
{
	private final static int state_created = 0;
	private final static int state_waiting = 1;
	private final static int state_playing = 2;
	private final static int state_done = 3;

	public final static int event_state = 0;
	public final static int event_join = 1;
	public final static int event_leave = 2;
	public final static int event_log = 3;
	public final static int event_data = 4;
	public final static int event_error = 5;

	/*private final static int type_ggz = 0;
	private final static int type_game = 0;*/

	//private RandomAccessFile f;
	//private int type;
	private int state;
	private int fd = -1;
	private int num_seats;
	private int num_open;
	//list seats;
	//callbacks? mod_handlers
	//gamedata?

	public GGZDModHandler()
	{
	}

	public void connect(int fd)
	{
		this.fd = fd;
		//f = new RandomAccessFile(fd);
	}

	public void disconnect() throws IOException
	{
		fd = -1;
		fd = req_game_state;
		// int: req_game_state, char: state_done
		// java: char = 16 bits, byte = 8 bits, int = 32 bitsi

		//f.writeInt(req_game_state);
		//f.writeByte(state_done);
	}
}

// Game server class
public abstract class GGZDMod
{
	private GGZDModHandler handler;

	private void log(String message)
	{
		System.out.println("ggzdmod> " + message);
	}

	public GGZDMod(String gamename)
	{
		log("create game " + gamename);
		handler = new GGZDModHandler();
	}

	public void connect()
	{
		log("connect");
		handler.connect(3);
	}

	public void disconnect()
	{
		log("disconnect");
		try
		{
			handler.disconnect();
		}
		catch(IOException e)
		{
			log("disconnect failed: " + e);
		}
	}

	private void handle()
	{
		int event = 0;
		GGZDModSeat seat;
		byte[] data;

		switch(event)
		{
			case GGZDModHandler.event_state:
				break;
			case GGZDModHandler.event_join:
				seat = new GGZDModSeat();
				joinEvent(seat);
				break;
			case GGZDModHandler.event_leave:
				seat = new GGZDModSeat();
				leaveEvent(seat);
				break;
			case GGZDModHandler.event_data:
				data = new byte[10];
				dataEvent(data);
				break;
			case GGZDModHandler.event_error:
				break;
		}
	}

	public void loop()
	{
		log("enter loop");
		while(true)
		{
			handle();
		}
	}

	abstract protected void joinEvent(GGZDModSeat seat);
	abstract protected void leaveEvent(GGZDModSeat seat);
	abstract protected void dataEvent(byte[] data);
}

