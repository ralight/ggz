// GGZ Java Server
// An attemt to build a distributed GGZ service
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions

// Java Namespaces
import java.io.*;
import java.util.*;
import java.net.*;

// Server information class: represents a remote server
class ServerInfo
{
	private String name;
	private int port;
	private String[] rooms;

	// Constructor: assign server parameters
	public ServerInfo(String name, int port)
	{
		this.name = name;
		this.port = port;
		this.rooms = null;
	}
}

// Main server class: connection handling and thread organization
class Server
{
	static Vector threads = new Vector();
	static Vector servers = new Vector();

	// Initialize the server and preload all threads
	static void init()
	{
		for(int i = 0; i < 3; i++)
		{
			Worker w = new Worker();
			Thread t = new Thread("worker #" + i);
			t.start();
			threads.addElement(w);
		}

		ServerInfo si = new ServerInfo("localhost", 10002);
		servers.addElement(si);
		ServerInfo si2 = new ServerInfo("localhost", 10003);
		servers.addElement(si2);
	}

	// Main loop
	static void work() throws IOException
	{
		ServerSocket sock = new ServerSocket(10000);
		while(true)
		{
			Socket s = sock.accept();
			System.out.println("==> Connection from: <unknown> on socket " + s);

			synchronized (threads)
			{
				if(threads.isEmpty())
				{
					System.out.println("<== Refused (too many open connections)");
				}
				else
				{
					Worker w = (Worker) threads.elementAt(0);
					threads.removeElementAt(0);
					w.setSocket(s);
					w.run();
				}
			}
		}
	}

	// Constructor
	public Server() throws IOException
	{
		System.out.println("GGZ Java Server");

		init();
		work();
	}

	// Server entry point
	public static void main(String[] args) throws IOException
	{
		Server foo = new Server();
	}
}

// Worker class: represents a client connection
class Worker
{
	private Socket s;

	// Receive the socket from the server object
	void setSocket(Socket s)
	{
		this.s = s;
	}

	// Actually handle the client
	void run()
	{
		System.out.println("Worker: Running on socket " + s);
	}
}

