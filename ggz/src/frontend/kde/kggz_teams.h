#ifndef KGGZ_TEAMS_H
#define KGGZ_TEAMS_H

#include <qwidget.h>

class KGGZ_Teams : public QWidget
{
Q_OBJECT
public:
	KGGZ_Teams(QWidget *parent, char *name);
	~KGGZ_Teams();
protected slots:
	void done();
};

#endif
