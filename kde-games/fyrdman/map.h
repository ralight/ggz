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

	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);

	private:
		void setupMap(int x, int y);

		int m_width, m_height;
		int m_map, m_knights, m_possession, m_animation;
};

#endif

