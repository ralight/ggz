#ifndef FYRDMAN_UNITINFO_H
#define FYRDMAN_UNITINFO_H

#include <qdialog.h>

class QFrame;

class UnitInfo : public QDialog
{
	Q_OBJECT
	public:
		UnitInfo(int num, QWidget *parent = NULL, const char *name = NULL);
		~UnitInfo();

		void display(int num);

	private:
		QFrame *frame;
};

#endif

