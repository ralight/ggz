#ifndef GATHERING_H
#define GATHERING_H

#include <qwidget.h>

class Gathering : public QWidget
{
Q_OBJECT
public:
	Gathering(QWidget *parent, char *name);
	~Gathering();
};

#endif
