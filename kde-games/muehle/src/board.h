#ifndef BOARD_H
#define BOARD_H

#include <qwidget.h>
#include <qlist.h>

class Stone;
class QWeb;
class QPixmap;
class Net;
class QPainter;

class Board : public QWidget
{
	Q_OBJECT
	public:
		Board(QWidget *parent = NULL, const char *name = NULL);
		~Board();
		void init();
		void remis();
		void loose();
		void setTheme(QString theme);
		void setVariant(QString variant);

	protected:
		void paintEvent(QPaintEvent *e);
		void resizeEvent(QResizeEvent *e);
		void mousePressEvent(QMouseEvent *e);

	private:
		void paintStone(QPixmap *tmp, QPainter *p, int x, int y, int owner);

		QList<Stone> stonelist;
		QWeb *web;
		QPixmap *bg;
		QPixmap *black, *white;
		Net *net;
};

#endif

