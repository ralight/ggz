#ifndef KGGZ_SPLASH_H
#define KGGZ_SPLASH_H

#include <qwidget.h>

class KGGZSplash : public QWidget
{
	Q_OBJECT
	public:
		KGGZSplash(QWidget *parent = NULL, char *name = NULL);
		~KGGZSplash();
};

#endif

