#ifndef KGGZ_GAMES_H
#define KGGZ_GAMES_H

#include <qtable.h>

class KGGZ_Games : public QTable
{
Q_OBJECT
public:
	KGGZ_Games(QWidget *parent, char *name);
	~KGGZ_Games();
	void addGame(char *name, char *author, char *version, char *frontend, char *homepage);
private:
	int m_gamenum;
};

#endif
