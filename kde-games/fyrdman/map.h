#ifndef FYRDMAN_MAP_H
#define FYRDMAN_MAP_H

#include <qwidget.h>

class Map : public QWidget
{
	Q_OBJECT
	public:
		Map(QWidget *parent = NULL, const char *name = NULL);
		~Map();

		void setMap(bool map);
		void setKnights(bool knights);
		void setPossession(bool possession);
		void setAnimation(bool animation);

	signals:
		void signalMove(int x, int y, int x2, int y2);

	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);

	private:
		void setupMap(int x, int y);

		int m_width, m_height;
		bool m_map, m_knights, m_possession, m_animation;
		bool m_picked;
		int m_x, m_y, m_x2, m_y2;
};

#endif

