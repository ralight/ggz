#ifndef DLG_AGAIN_H
#define DLG_AGAIN_H

#include <qwidget.h>

class DlgAgain : public QWidget
{
	Q_OBJECT
	public:
		DlgAgain(QWidget *parent, char *name);
		~DlgAgain();
	protected:
		void paintEvent(QPaintEvent *e);
};

#endif
