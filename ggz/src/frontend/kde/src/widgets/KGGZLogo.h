#ifndef KGGZ_LOGO_H
#define KGGZ_LOGO_H

#include <qevent.h>
#include <qframe.h>

class KGGZLogo : public QFrame
{
	Q_OBJECT
	public:
		KGGZLogo(QWidget *parent = NULL, char *name = NULL);
		~KGGZLogo();
		void setLogo(char *logo, char *name);

	signals:
		void signalInfo();

	protected:
		void mousePressEvent(QMouseEvent *e);
};

#endif
