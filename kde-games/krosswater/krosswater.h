#ifndef KROSSWATER_H
#define KROSSWATER_H

#include "qcw.h"
#include "dlg_again.h"
#include <ZoneGGZModUI.h>
#include <qframe.h>
#include <qevent.h>

class Krosswater : public ZoneGGZModUI
{
	Q_OBJECT
	public:
		Krosswater(QWidget *parent, char *name);
		~Krosswater();

		enum KrosswaterProto
		{
			proto_helloworld = 101,
			proto_move = 102,
			proto_map = 103,
			proto_map_respond = 104,
			proto_map_backtrace = 105,
			proto_move_broadcast = 106
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

	private:
		void showStatus(const char *state);

		QCw* qcw;
		int m_fromx, m_fromy, m_tox, m_toy;
		const char *m_currentstate;
		QFrame *m_statusframe;
		int m_selectedperson;
		DlgAgain *m_again;
};

#endif
