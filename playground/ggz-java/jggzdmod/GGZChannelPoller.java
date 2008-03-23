// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

// Copy of GGZChannel for GGZDMod

package GGZDMod;

import java.util.HashMap;
import java.util.Set;

public class GGZChannelPoller
{
	private HashMap polls;

	native int polldescriptors(int[] fds);

	public GGZChannelPoller()
	{
		this.polls = new HashMap();
	}

	public void registerChannel(GGZChannel channel)
	{
		this.polls.put(new Integer(channel.getFd()), channel);
	}

	public GGZChannel poll()
	{
		Set keys = this.polls.keySet();
		Object[] array = keys.toArray();
		int[] fds = new int[array.length];
		for(int i = 0; i < array.length; i++)
			fds[i] = ((Integer)array[i]).intValue();
System.out.println("### assigned " + array.length);
		int fd = polldescriptors(fds);
System.out.println("### polled " + fd);
		for(int i = 0; i < array.length; i++)
			if(((Integer)array[i]).intValue() == fd)
				return (GGZChannel)this.polls.get((Integer)array[i]);
		return null;
	}

	static
	{
		System.loadLibrary("jancillary");
	}
}

