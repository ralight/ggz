/////////////////////////////////////////////////////////////////////
//
// Kryds: Tic-Tac-Toe game server for KDE 4
// http://www.ggzgamingzone.org/gameservers/kryds/
//
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KRYDS_H
#define KRYDS_H

#include "tictactoe_server.h"

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
		void slotNotification(tictactoeOpcodes::Opcode messagetype, const msg& message);
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
