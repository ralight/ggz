#ifndef FYRDMAN_MAP_H
#define FYRDMAN_MAP_H

#include <qwidget.h>

class Level;

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
		void setBackground(QString background);

		void setupMap(Level *level);
		Level *level();

		void move(int x, int y, int x2, int y2);

	signals:
		void signalMove(int x, int y, int x2, int y2);

	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);

	private:
		int m_width, m_height;
		bool m_map, m_knights, m_possession, m_animation;
		bool m_picked;
		int m_x, m_y, m_x2, m_y2;
		Level *m_level;
		QString m_background;
};

#endif

