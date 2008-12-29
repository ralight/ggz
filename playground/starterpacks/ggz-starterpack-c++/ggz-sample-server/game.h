//
// GGZ Starterpack for C++ - Sample Server
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#ifndef SAMPLE_GAME_H
#define SAMPLE_GAME_H

#include "net.h"

#include <QObject>

#include <kggzdmod/event.h>

namespace KGGZdMod
{
	class Module;
	class Player;
};

class Kryds : public QObject
{
	Q_OBJECT
	public:
		Kryds(QObject *parent = NULL);
	private slots:
 		void slotEvent(const KGGZdMod::Event& event);
		void slotNotification(ggz_starterpackOpcodes::Opcode messagetype, const msg& message);
		void slotError();
	private:
		void nextPlayer();
		void detectGameOver();
		void handleInput(KGGZdMod::Player *p);
		void shutdown();

		KGGZdMod::Module *m_module;
		KGGZdMod::Player *m_currentplayer;
		bool m_started;
		int m_turn;
		int m_board[9];
};

#endif
