#ifndef OPTIONS_H
#define OPTIONS_H

#include <qwidget.h>

class Options : public QWidget
{
	Q_OBJECT
	public:
		Options(QWidget *parent = NULL, const char *name = NULL);
		~Options();
	public slots:
		void slotTime();
	signals:
		void signalTime(int time);
};

#endif

