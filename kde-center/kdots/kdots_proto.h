/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_PROTO_H
#define KDOTS_PROTO_H

//#include <ggzmod.h>
#include <qglobal.h>

#define MAX_WIDTH 25
#define MAX_HEIGHT 25

namespace KGGZMod
{
	class Module;
};

class KDots;
class KGGZPacket;

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
		//static void handle_server(GGZMod *mod, GGZModEvent e,
		//			  const void *data);

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

		//GGZMod *getMod();
		
		int state;
		//int fd;
		//int fdcontrol;
		int num;
		char players[2][32];
		qint8 width, height;
		int turn;
		int movex, movey;
		int m_lastx, m_lasty;
		int m_lastdir;

	private:
		//GGZMod *mod;
		//KDots *gameobject;
		//static KDotsProto *self;
		//KGGZMod::Module *m_mod;
		KGGZPacket *m_packet;
};

#endif

