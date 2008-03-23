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

	private final static byte STATUS_OK = 0;
	private final static byte STATUS_ERR_STATE = -1;
	private final static byte STATUS_ERR_TURN = -2;
	private final static byte STATUS_ERR_BOUND = -3;
	private final static byte STATUS_ERR_FULL = -4;

	private final static int FIELD_EMPTY = -1;
	private final static byte NO_WINNER = 2;

	private int[] board;
	private int turn;

	public TTTServer()
	throws Exception
	{
		super("Tic-Tac-Toe (java)");

		this.board = new int[9];
		for(int i = 0; i < 9; i++)
			board[i] = FIELD_EMPTY;
		this.turn = 0;
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
		try
		{
			dataEventPrivate(seat);
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	protected void seatEvent(Seat oldseat, Seat newseat)
	{
		try
		{
			seatEventPrivate(oldseat, newseat);
		}
		catch(Exception e)
		{
			log("ERROR: " + e.toString());
		}
	}

	private void seatEventPrivate(Seat oldseat, Seat newseat)
	throws Exception
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

			p1.getClient().writeInt(REQMOVE);
		}
	}

	private void dataEventPrivate(Seat seat)
	throws Exception
	{
		log("data: " + seat.getName());

		// might be spectator or player
		GGZChannel client = ((Spectator)seat).getClient();

		int op;
		op = client.readInt();

		if(op == SNDMOVE)
		{
			int move = client.readInt();
			byte status = STATUS_OK;

			if((move < 0) || (move >= 9))
			{
				status = STATUS_ERR_BOUND;
			}
			else if(getState() != STATE_PLAYING)
			{
				status = STATUS_ERR_STATE;
			}
			else if(this.board[move] != FIELD_EMPTY)
			{
				status = STATUS_ERR_FULL;
			}
			else if(this.turn != seat.getNum())
			{
				status = STATUS_ERR_TURN;
			}
			else
			{
				this.board[move] = seat.getNum();
			}

			// poor man's TTT rules
			boolean over = true;
			for(int i = 0; i < 9; i++)
				if(this.board[i] == FIELD_EMPTY)
					over = false;

			client.writeInt(RSPMOVE);
			client.writeByte(status);

			this.turn = 1 - this.turn;

			Player opponent = (Player)getSeats().get(this.turn);
			GGZChannel oppchannel = opponent.getClient();
			if(oppchannel != null)
			{
				client.writeInt(MSGMOVE);
				client.writeInt(1 - this.turn);
				client.writeInt(move);
			}

			if(over)
			{
				// FIXME: broadcast
				client.writeInt(MSGGAMEOVER);
				client.writeByte(NO_WINNER);
			}
		}
		else if(op == REQSYNC)
		{
			client.writeInt(SNDSYNC);
			client.writeByte((byte)this.turn);
			for(int i = 0; i < 9; i++)
				client.writeInt(this.board[i]);
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

