#ifndef KGGZ_TABLES_H
#define KGGZ_TABLES_H

#include <qiconview.h>

class KGGZTables : public QIconView
{
Q_OBJECT
public:
	KGGZTables(QWidget *parent, char *name);
	~KGGZTables();
	void reset();
	void add(char *name, int used, int total);
};

#endif
