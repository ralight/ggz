#ifndef KRYDS_H
#define KRYDS_H

#include <QObject>

#include <kggzdmod/event.h>

namespace KGGZdMod
{
	class Module;
};

class Kryds : public QObject
{
	Q_OBJECT
	public:
		Kryds(QObject *parent = NULL);
	private slots:
 		void slotEvent(const KGGZdMod::Event& event);
	private:
		void nextPlayer();
		void detectGameOver();

		KGGZdMod::Module *m_module;
		bool m_started;
		int m_turn;
		int m_board[9];
};

#endif
