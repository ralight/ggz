#ifndef QCW_H
#define QCW_H

#include <qwidget.h>
#include <qevent.h>
#include <qtimer.h>

class QCw : public QWidget
{
	Q_OBJECT
	public:
		QCw(QWidget* parent = 0, char* name = 0);
		~QCw();
		void setSize(int width, int height);
		void setStone(int x, int y, int value);
		void setStoneState(int x, int y, int state);

		void resetPlayers();		
		void addPlayer(int x, int y);
		void setPlayerPixmap(int player, int pixmap);
	signals:
		void signalMove(int xfrom, int yfrom, int xto, int yto);
	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);
		void timerEvent(QTimerEvent *e);
	private:
		enum states
		{
			normal,
			selected
		};
		int m_width, m_height;
		int ***m_board;
		int m_x, m_y;
		int m_state;
		int m_players[4][3];
		int m_numplayers;
};

#endif
