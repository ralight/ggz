#ifndef KGGZ_ABOUT_H
#define KGGZ_ABOUT_H

#include <qwidget.h>
#include <qtimer.h>
#include <qframe.h>

class KGGZAbout : public QWidget
{
	Q_OBJECT
	public:
		KGGZAbout(QWidget *parent = NULL, char *name = NULL);
		~KGGZAbout();

	protected slots:
		void timerEvent(QTimerEvent *e);
		void paintEvent(QPaintEvent *e);

	private:
		QFrame *m_frame;
};

#endif

