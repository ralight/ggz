#ifndef KGGZ_TABLES_H
#define KGGZ_TABLES_H

#include <qiconview.h>

class KGGZ_Tables : public QIconView
{
Q_OBJECT
public:
	KGGZ_Tables(QWidget *parent, char *name);
	~KGGZ_Tables();
};

#endif
