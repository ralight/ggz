#ifndef CHESS_OPTIONS_H
#define CHESS_OPTIONS_H

#include <qwidget.h>

class QRadioButton;
class QSpinBox;

class Options : public QWidget
{
	Q_OBJECT
	public:
		Options(QWidget *parent = NULL, const char *name = NULL);
		~Options();
	public slots:
		void slotTime();
	signals:
		void signalTime(int timeoption, int time);
	private:
		QRadioButton *noclock, *serverclock, *clientclock;
		QSpinBox *minutes, *seconds;
};

#endif

