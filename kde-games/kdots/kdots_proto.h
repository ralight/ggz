///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_PROTO_H
#define KDOTS_PROTO_H

#include <ggzmod.h>

#define MAX_WIDTH 25
#define MAX_HEIGHT 25

class KDots;

class KDotsProto
{
	public:
		KDotsProto(KDots *game);
		~KDotsProto();

		enum Errors
		{
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errfull = -4
		};

		enum ServerMessages
		{
			msgseat = 0,
			msgplayers = 1,
			msgmoveh = 2,
			msgmovev = 3,
			msggameover = 4,
			reqmove = 5,
			rspmove = 6,
			sndsync = 7,
			msgoptions = 8,
			reqoptions = 9
		};

		enum ClientMessages
		{
			sndmoveh = 0,
			sndmovev = 1,
			reqsync = 2,
			sndoptions = 3,
			reqnewgame = 4
		};

		enum States
		{
			stateinit = 0,
			statewait = 1,
			statemove = 2,
			statedone = 3,
			stateopponent = 4,
			statechoose = 5
		};

		enum Events
		{
			eventlaunch = 0,
			eventjoin = 1,
			eventleave = 2,
			eventmoveh = 3,
			eventmovev = 4
		};

		void connect();
		void disconnect();
		void init();
		void sync();
		void dispatch();
		static void handle_server(GGZMod *mod, GGZModEvent e,
					  const void *data);

		void getPlayers();
		void getSeat();
		void sendMove(int x, int y, int direction);
		void sendOptions(int wantwidth, int wantheight);
		void getOptions();
		int getMove();
		int getOpcode();
		int getStatus();
		int getSyncMove();
		int getSyncScore();
		void getOppMove(int direction);
		
		int state;
		int fd;
		int fdcontrol;
		int num;
		char players[2][32];
		char width, height;
		int turn;
		int movex, movey;
		int m_lastx, m_lasty;
		int m_lastdir;

	private:
		GGZMod *mod;
		KDots *gameobject;
		static KDotsProto *self;
};

#endif

