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

class Game : public QObject
{
	Q_OBJECT
	public:
		Game(QObject *parent = NULL);

	private slots:
 		void slotEvent(const KGGZdMod::Event& event);
		void slotNotification(ggz_starterpackOpcodes::Opcode messagetype, const msg& message);
		void slotError();

	private:
		void nextPlayer();
		void detectGameOver();
		void handleInput(KGGZdMod::Player *p);
		void shutdown();

		bool allSeatsFull();
		KGGZdMod::Player *findPlayerBySeat(int seatnum);

		KGGZdMod::Module *m_module;
};

#endif
