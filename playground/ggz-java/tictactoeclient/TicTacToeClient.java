import java.awt.*;
import java.io.*;

class TTTAbout extends Dialog
{
	TTTAbout(Frame parent)
	{
		//super(parent, "About GGZ Java TicTacToe Client", true);
		super(parent);
		setBackground(Color.blue);
		setForeground(Color.white);
		setSize(400, 150);

		Font f = new Font("TimesRoman", Font.PLAIN, 12);
		setFont(f);

		GridLayout layout = new GridLayout(5, 1);
		setLayout(layout);
		add(new Label("A java client for the GGZ TicTacToe game"));
		add(new Label("Copyright (C) 2002 Josef Spillner"));
		add(new Label("dr_maux@users.sourceforge.net"));
		add(new Label("Published under GNU GPL conditions"));
		add(new Button("Close"));
	}

	public boolean action(Event e, Object o)
	{
		if(o.equals("Close"))
		{
			dispose();
			return true;
		}
		else return super.handleEvent(e);
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

	TTTProto()
	{
	}
}

public class TicTacToeClient extends Frame implements Runnable
{
	TicTacToeClient()
	{
		super("GGZ Java TicTacToe Client");
		setSize(400, 400);
		setBackground(Color.blue);
		setForeground(Color.yellow);

		Font font = new Font("TimesRoman", Font.PLAIN, 22);
		setFont(font);

		GridLayout layout = new GridLayout(3, 3);
		setLayout(layout);
		add(new Button(""));
		add(new Button("X"));
		add(new Button(""));
		add(new Button("O"));
		add(new Button(""));
		add(new Button("X"));
		add(new Button(""));
		add(new Button(""));
		add(new Button(""));

		initGui();

		loop();
	}

	private void loop()
	{
		Thread t = new Thread(this);
		t.start();
	}

	public void run()
	{
		FileInputStream s = new FileInputStream(FileDescriptor.in);
		TTTProto proto = new TTTProto();
		int x = 0;

		Thread.currentThread().setPriority(Thread.MIN_PRIORITY);

		while(true)
		{
			try
			{
				x = s.available();
			}
			catch(IOException ex)
			{
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
				}
				for(int i = 0; i < x; i++)
				{
					System.out.println("Char: " + b[i]);
				}

				int op = b[0] + (b[1] << 8) + (b[2] << 16) + (b[3] << 24);
				switch(op)
				{
					case TTTProto.msgseat:
						System.err.println(">> msgseat");
						break;
					case TTTProto.msgplayers:
						System.err.println(">> msgplayers");
						break;
					case TTTProto.msgmove:
						System.err.println(">> msgmove");
						break;
					case TTTProto.msggameover:
						System.err.println(">> msggameover");
						break;
					case TTTProto.reqmove:
						System.err.println(">> reqmove");
						break;
					case TTTProto.rspmove:
						System.err.println(">> rspmove");
						break;
					case TTTProto.sndsync:
						System.err.println(">> sndsync");
						break;
					default:
						System.err.println(">> ERROR: Unknown opcode " + op);
				}
			}
		}
	}

	private void initGui()
	{
		MenuBar menubar = new MenuBar();

		Menu game = new Menu("Game");
		game.add(new MenuItem("Exit"));

		Menu help = new Menu("Help");
		help.add(new MenuItem("About"));

		menubar.add(game);
		menubar.add(help);

		Font font = new Font("TimesRoman", Font.PLAIN, 12);
		menubar.setFont(font);

		setMenuBar(menubar);
	}

	public boolean action(Event e, Object o)
	{
		if(o.equals("Exit"))
		{
			System.exit(-1);
			return true;
		}
		else if(o.equals("About"))
		{
			TTTAbout a = new TTTAbout(this);
			a.show();
			return true;
		}
		else if(e.target instanceof Button)
		{
			return true;
		}
		else return super.handleEvent(e);
	}

	public boolean handleEvent(Event e)
	{
		switch(e.id)
		{
			case Event.WINDOW_DESTROY:
				System.exit(0);
				return true;
			default:
				return super.handleEvent(e);
		}
	}

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
			System.out.println("This game is not intended to be run directly.");
			System.out.println("Please launch it from a GGZ core client.");
			System.exit(0);
		}

		TicTacToeClient ttt = new TicTacToeClient();
		ttt.show();
	}
}

