// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

abstract class Protocol
{
	// from game server to GGZ table
	protected final static int MSG_LOG = 0;
	protected final static int REQ_GAME_STATE = 1;
	protected final static int REQ_NUM_SEATS = 2;
	protected final static int REQ_BOOT = 3;
	protected final static int REQ_BOT = 4;
	protected final static int REQ_OPEN = 5;
	protected final static int MSG_GAME_REPORT = 6;
	protected final static int MSG_SAVEGAME_REPORT = 7;

	// from GGZ table to game server
	protected final static int MSG_GAME_LAUNCH = 0;
	protected final static int MSG_GAME_SEAT = 1;
	protected final static int MSG_GAME_SPECTATOR_SEAT = 2;
	protected final static int MSG_GAME_RESEAT = 3;
	protected final static int RSP_GAME_STATE = 4;
	protected final static int MSG_SAVEDGAMES = 5;

	// states
	public final static int STATE_CREATED = 0;
	public final static int STATE_WAITING = 1;
	public final static int STATE_PLAYING = 2;
	public final static int STATE_DONE = 3;
	public final static int STATE_RESTORED = 4;
}

