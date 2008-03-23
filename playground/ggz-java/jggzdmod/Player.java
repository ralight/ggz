// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

public class Player extends Spectator implements Seat
{
	public final static int TYPE_OPEN = 1;
	public final static int TYPE_BOT = 2;
	public final static int TYPE_PLAYER = 3;
	public final static int TYPE_RESERVED = 4;
	public final static int TYPE_ABANDONED = 5;

	private int type;

	public Player(String name, int num, int fd, int type)
	{
		super(name, num, fd);
		this.type = type;
	}

	public int getType()
	{
		return this.type;
	}
}

