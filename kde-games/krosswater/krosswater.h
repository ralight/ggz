// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef KROSSWATER_H
#define KROSSWATER_H

// Krosswater includes
#include "qcw.h"
#include "dlg_again.h"

// Zone includes
#include <ZoneGGZModUI.h>

// Qt includes
#include <qframe.h>
#include <qevent.h>
#include <qptrlist.h>

// Forward includes
class KrosswaterMove;

// Main Krosswater widget
class Krosswater : public ZoneGGZModUI
{
	Q_OBJECT
	public:
		Krosswater(QWidget *parent = NULL, const char *name = NULL);
		~Krosswater();

		enum KrosswaterProto
		{
			proto_helloworld = 101,
			proto_move = 102,
			proto_map = 103,
			proto_map_respond = 104,
			proto_map_backtrace = 105,
			proto_move_broadcast = 106,
			proto_restart = 107
		};

		enum SleepStates
		{
			state_sleep1,
			state_sleep2,
			state_sleep3,
			state_sleep4,
			state_nosleep
		};

	public slots:
		void slotSelected(int person);
		void slotMove(int fromx, int fromy, int tox, int toy);
		void slotZoneInput(int op);
		void slotZoneReady();
		void slotZoneTurn();
		void slotZoneTurnOver();
		void slotZoneOver();
		void slotZoneInvalid();
		void slotZoneBroadcast();
		void slotMenuQuit();
		void slotMenuAbout();
		void slotMenuHelp();
		void slotAgain();

	protected:
		void paintEvent(QPaintEvent *e);
		void timerEvent(QTimerEvent *e);

	private:
		void showStatus(QString state);
		void protoError();

		QCw* qcw;
		int m_fromx, m_fromy, m_tox, m_toy;
		QString m_currentstate;
		QFrame *m_statusframe;
		int m_selectedperson;
		DlgAgain *m_again;
		int m_broken;
		int m_sleep;
		QPtrList<KrosswaterMove> m_movelist;
		int m_turn;
};

#endif

