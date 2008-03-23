// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

public class Spectator implements Seat
{
	private String name;
	private int num;
	private int fd;

	public Spectator(String name, int num, int fd)
	{
		this.name = name;
		this.num = num;
		this.fd = fd;
	}

	public String getName()
	{
		return this.name;
	}

	public int getNum()
	{
		return this.num;
	}

	public int getFd()
	{
		return this.fd;
	}
}

