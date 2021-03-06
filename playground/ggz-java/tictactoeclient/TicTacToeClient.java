import java.awt.*;
import java.io.*;
import java.awt.event.*;

class TTTAbout extends Dialog implements ActionListener
{
	Button closebutton;

	TTTAbout(Frame parent)
	{
		super(parent, "About GGZ Java TicTacToe Client", true);
		setBackground(Color.blue);
		setForeground(Color.white);
		setSize(600, 150);

		Font f = new Font("TimesRoman", Font.PLAIN, 12);
		setFont(f);

		GridLayout layout = new GridLayout(5, 1);
		setLayout(layout);
		add(new Label("A java client for the GGZ TicTacToe game"));
		add(new Label("Copyright (C) 2002 - 2004 Josef Spillner <josef@ggzgamingzone.org>"));
		add(new Label("Published under GNU GPL conditions"));
		closebutton = new Button("Close");
		add(closebutton);

		closebutton.addActionListener(this);
	}

	public void actionPerformed(ActionEvent e)
	{
		if(e.getSource() == closebutton)
		{
			dispose();
			return;
		}
	}
}

class TTTOver extends Dialog implements ActionListener
{
	Button closebutton;

	TTTOver(Frame parent, byte winner)
	{
		super(parent, "Game over", true);
		setBackground(Color.blue);
		setForeground(Color.white);
		setSize(600, 150);

		Font f = new Font("TimesRoman", Font.PLAIN, 12);
		setFont(f);

		GridLayout layout = new GridLayout(3, 1);
		setLayout(layout);
		if(winner == 2)
			add(new Label("You won."));
		else
			add(new Label("You lost."));
		closebutton = new Button("Close");
		add(closebutton);

		closebutton.addActionListener(this);
	}

	public void actionPerformed(ActionEvent e)
	{
		if(e.getSource() == closebutton)
		{
			dispose();
			return;
		}
	}
}

class TTTProto
{
	public static final int msgseat = 0;
	public static final int msgplayers = 1;
	public static final int msgmove = 2;
	public static final int msggameover = 3;
	public static final int reqmove = 4;
	public static final int rspmove = 5;
	public static final int sndsync = 6;
	public static final int sndstats = 7;

	public static final int sndmove = 0;

	public static final int SEAT_PLAYER = 3;
	public static final int SEAT_BOT = 2;

	TTTProto()
	{
	}
}

class TicTacToeEngine implements Runnable
{
	TicTacToeManager mManager;
	FileInputStream s;
	FileOutputStream o;
	TTTProto proto;
	byte[] input = null;
	byte oldmove;

	TicTacToeEngine(TicTacToeManager manager)
	{
		manager.setEngine(this);
		mManager = manager;
	}

	public void readNetwork()
	{
		int x;

		try
		{
			x = s.available();
		}
		catch(IOException ex)
		{
			x = -1;
		}
		if(x > 0)
		{
			byte[] b = new byte[x];
			try
			{
				s.read(b);
			}
			catch(IOException ex)
			{
				x = -1;
			}
//			for(int i = 0; i < x; i++)
//			{
//				System.err.println("Char: " + b[i]);
//			}

			int tmplength = 0;
			byte[] tmp = null;
			if(input != null)
			{
				tmp = new byte[input.length];
				for(int i = 0; i < tmp.length; i++)
					tmp[i] = input[i];
				tmplength = tmp.length;
			}
			input = new byte[tmplength + b.length];
			if(tmp != null)
			{
				for(int i = 0; i < tmplength; i++)
					input[i] = tmp[i];
			}
			for(int i = tmplength; i < tmplength + b.length; i++)
				input[i] = b[i - tmplength];
		}
	}

	public int readAvailable()
	{
		readNetwork();

		if(input == null) return 0;
		return input.length;
	}

	public void cutArray(int cut)
	{
		byte[] tmp = new byte[input.length];
		for(int i = 0; i < tmp.length; i++)
			tmp[i] = input[i];

		input = new byte[tmp.length - cut];
		for(int i = cut; i < tmp.length; i++)
			input[i - cut] = tmp[i];
	}

	public byte readChar()
	{
		readNetwork();
		if(input.length >= 1)
		{
			byte op = input[0];
			cutArray(1);
			return op;
		}
		else System.err.println("readChar(): buffer underrun");
		return 0;
	}

	public int readInt()
	{
		readNetwork();
		if(input.length >= 4)
		{
			int op = input[3] + (input[2] << 8) + (input[1] << 16) + (input[0] << 24);
			cutArray(4);
			return op;
		}
		else System.err.println("readInt(): buffer underrun");
		return 0;
	}

	public String readString()
	{
		int len = readInt();

		if(input.length >= len)
		{
			char[] str = new char[len];
			for(int i = 0; i < len; i++)
				str[i] = (char)input[i];
			cutArray(len);
			return new String(str);
		}
		else System.err.println("readString(): buffer underrun");
		return "";
	}

	public void run()
	{
		s = new FileInputStream(FileDescriptor.in);
		o = new FileOutputStream(FileDescriptor.out);

		proto = new TTTProto();
		Thread.currentThread().setPriority(Thread.MIN_PRIORITY);

		while(true)
		{
			if(readAvailable() == 0) continue;

			int op = readInt();
			System.err.println("- OPCODE: " + op);
			switch(op)
			{
				case TTTProto.msgseat:
					System.err.println(">> msgseat");
					int seat = readInt();
					System.err.println("SEAT: " + seat);
					break;
				case TTTProto.msgplayers:
					System.err.println(">> msgplayers");
					for(int i = 0; i < 2; i++)
					{
						int seattype = readInt();
						String seatname;
						if((seattype == TTTProto.SEAT_PLAYER) || (seattype == TTTProto.SEAT_BOT))
							seatname = readString();
						else seatname = null;
						System.err.println("PLAYER: " + seattype + seatname);
					}
					break;
				case TTTProto.msgmove:
					System.err.println(">> msgmove");
					int nummove = readInt();
					int move = readInt();
					System.err.println("MOVE: " + nummove + move);
					byte[] msg = new byte[2];
					msg[0] = 1;
					msg[1] = (byte)move;
					mManager.sendMessage(this, msg);
					break;
				case TTTProto.msggameover:
					System.err.println(">> msggameover");
					byte winner = readChar();
					System.err.println("WINNER: " + winner);
					TTTOver a = new TTTOver(mManager.gui(), winner);
					a.show();
					break;
				case TTTProto.reqmove:
					System.err.println(">> reqmove");
					break;
				case TTTProto.rspmove:
					System.err.println(">> rspmove");
					byte status = readChar();
					System.err.println("MOVE STATUS: " + status);
					if(status == 0)
					{
						byte[] msg2 = new byte[2];
						msg2[0] = 0;
						msg2[1] = oldmove;
						mManager.sendMessage(this, msg2);
					}
					break;
				case TTTProto.sndsync:
					System.err.println(">> sndsync");
					break;
				default:
					System.err.println(">> ERROR: Unknown opcode " + op);
			}
		}
	}

	public void sendMessage(byte[] message)
	{
		for(int i = 0; i < message.length; i++)
			System.err.println("engine::message " + message[i]);
		//System.out.print(message);
		//System.out.flush();
		try
		{
			o.write(message, 0, message.length);
			o.flush();
			//o.close();
		}
		catch(IOException ex)
		{
		}

		oldmove = message[7]; // HACK!
	}
}

class TicTacToeGui extends Frame implements ActionListener
{
	MenuItem exitbutton;
	MenuItem aboutbutton;
	Button[] buttons;
	TicTacToeManager mManager;

	TicTacToeGui(TicTacToeManager manager)
	{
		super("GGZ Java TicTacToe Client");
		setSize(400, 400);
		setBackground(Color.blue);
		setForeground(Color.yellow);

		Font font = new Font("TimesRoman", Font.PLAIN, 22);
		setFont(font);

		GridLayout layout = new GridLayout(3, 3);
		setLayout(layout);

		initGui();

		manager.setGui(this);
		mManager = manager;
	}

	private void initGui()
	{
		MenuBar menubar = new MenuBar();

		Menu game = new Menu("Game");
		exitbutton = new MenuItem("Exit");
		game.add(exitbutton);

		Menu help = new Menu("Help");
		aboutbutton = new MenuItem("About");
		help.add(aboutbutton);

		menubar.add(game);
		menubar.add(help);

		Font font = new Font("TimesRoman", Font.PLAIN, 12);
		menubar.setFont(font);

		exitbutton.addActionListener(this);
		aboutbutton.addActionListener(this);

		setMenuBar(menubar);

		buttons = new Button[9];
		for(int i = 0; i < 9; i++)
		{
			Button ttt = new Button("");
			add(ttt);
			buttons[i] = ttt;
			ttt.addActionListener(this);
		}
	}

	public void actionPerformed(ActionEvent e)
	{
		if(e.getSource() == exitbutton)
		{
			System.exit(-1);
			return;
		}
		else if(e.getSource() == aboutbutton)
		{
			TTTAbout a = new TTTAbout(this);
			a.show();
			return;
		}
		else
		{
			for(int i = 0; i < 9; i++)
				if(e.getSource() == buttons[i])
				{
					byte[] message = new byte[8];
					message[0] = 0;
					message[1] = 0;
					message[2] = 0;
					message[3] = TTTProto.sndmove;
					message[4] = 0;
					message[5] = 0;
					message[6] = 0;
					message[7] = (byte)i;
					mManager.sendMessage(this, message);
				}
		}
	}

	public void sendMessage(byte[] message)
	{
		for(int i = 0; i < message.length; i++)
			System.err.println("gui::message " + message[i]);
		int move = message[1];
		int num = message[0];
		Button b = buttons[move];
		if(num == 0) b.setLabel("X");
		else b.setLabel("O");
	}
}

class TicTacToeManager
{
	TicTacToeGui mGui;
	TicTacToeEngine mEngine;

	TicTacToeManager()
	{
	}

	public void setGui(TicTacToeGui gui)
	{
		mGui = gui;
	}

	public TicTacToeGui gui()
	{
		return mGui;
	}

	public void setEngine(TicTacToeEngine engine)
	{
		mEngine = engine;
	}

	public void sendMessage(Object sender, byte[] message)
	{
		if(sender == mGui) mEngine.sendMessage(message);
		else mGui.sendMessage(message);
	}
}

public class TicTacToeClient
{
	public static void main(String[] args)
	{
		int ggz = 0;

		for(int i = 0; i < args.length; i++)
		{
			if(args[i].equals("--ggz"))
			{
				ggz = 1;
			}
		}

		if(ggz == 0)
		{
			System.err.println("This game is not intended to be run directly.");
			System.err.println("Please launch it from a GGZ core client.");
			System.exit(0);
		}

		TicTacToeManager manager = new TicTacToeManager();
		TicTacToeGui tttg = new TicTacToeGui(manager);
		tttg.show();
		TicTacToeEngine ttte = new TicTacToeEngine(manager);
		ttte.run();
	}
}

