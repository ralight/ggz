// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

// Copy of GGZChannel for GGZDMod

package GGZDMod;

import java.util.HashMap;
import java.util.Set;

/** @brief Main loop class for GGZDMod
 *
 * This class gets instantiated once by GGZDMod and multiplexes the input from
 * all the various channels: One from the game server to GGZ, and many from the
 * game server to the game clients.
 *
 * Attention: This class will only work on systems which have libggz
 * installed, as it implements some native methods.
 */
public class GGZChannelPoller
{
	private HashMap polls;

	/** @internal */
	native int polldescriptors(int[] fds);

	/** @brief Constructor
	 *
	 * Instantiates a poller object.
	 */
	public GGZChannelPoller()
	{
		this.polls = new HashMap();
	}

	/** @brief Registers a communication channel
	 *
	 * The channel will be added to the polling process.
	 */
	public void registerChannel(GGZChannel channel)
	{
		this.polls.put(new Integer(channel.getFd()), channel);
	}

	/** @brief Polls all channels
	 *
	 * This method sleeps and doesn't return until a channel offers data
	 * to read.
	 *
	 * @return Activated channel
	 */
	public GGZChannel poll()
	{
		Set keys = this.polls.keySet();
		Object[] array = keys.toArray();
		int[] fds = new int[array.length];
		for(int i = 0; i < array.length; i++)
			fds[i] = ((Integer)array[i]).intValue();
		int fd = polldescriptors(fds);
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

