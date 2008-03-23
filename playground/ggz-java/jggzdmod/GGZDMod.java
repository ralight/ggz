// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

package GGZDMod;

public abstract class GGZDMod extends Handler
{
	public GGZDMod(String gamename)
	throws Exception
	{
		log("create game " + gamename);
		if(System.getenv("GGZMODE") == null)
		{
			throw new Exception("Game server not in GGZ mode.");
		}
		connect(new Integer(System.getenv("GGZSOCKET")).intValue());
	}

	public void log(String message)
	{
		super.log(message);
		System.out.println("ggzdmod> " + message);
	}

	public void loop()
	{
		log("enter loop");
		while(true)
		{
			try
			{
				handle();
			}
			catch(Exception e)
			{
				log(e.toString());
				break;
			}
		}
	}

	public String stateName(int state)
	{
		switch(state)
		{
			case STATE_CREATED:
				return "created";
			case STATE_WAITING:
				return "waiting";
			case STATE_PLAYING:
				return "playing";
			case STATE_DONE:
				return "done";
			case STATE_RESTORED:
				return "restored";
		}

		return null;
	}
}

