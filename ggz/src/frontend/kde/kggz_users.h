#ifndef KGGZ_USERS_H
#define KGGZ_USERS_H

#include <qlistview.h>
#include <qtimer.h>

class KGGZ_Users : public QListView
{
Q_OBJECT
public:
	KGGZ_Users(QWidget *parent, char *name);
	~KGGZ_Users();
	static void add(char *name);
protected:
	void timerEvent(QTimerEvent *e);
};

#endif
