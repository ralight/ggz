#ifndef DLG_ABOUT_H
#define DLG_ABOUT_H

#include <qwidget.h>
#include <qevent.h>

class DlgAbout : public QWidget
{
	Q_OBJECT
	public:
		DlgAbout(QWidget *parent, char *name);
		~DlgAbout();
	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);
};

#endif
