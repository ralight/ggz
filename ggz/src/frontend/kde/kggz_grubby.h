#ifndef KGGZ_GRUBBY_H
#define KGGZ_GRUBBY_H

#include <qwidget.h>

class KGGZ_Grubby : public QWidget
{
Q_OBJECT
public:
	KGGZ_Grubby(QWidget *parent, char *name);
	~KGGZ_Grubby();
protected slots:
	void slotClose();
};

#endif
