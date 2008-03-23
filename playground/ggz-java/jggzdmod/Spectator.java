// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

public class Spectator implements Seat
{
	private String name;
	private int num;
	private GGZChannel client;

	public Spectator(String name, int num, GGZChannel client)
	{
		this.name = name;
		this.num = num;
		this.client = client;
	}

	public String getName()
	{
		return this.name;
	}

	public int getNum()
	{
		return this.num;
	}

	public GGZChannel getClient()
	{
		return this.client;
	}
}

