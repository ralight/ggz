#ifndef DLG_PERSON_H
#define DLG_PERSON_H

#include <qwidget.h>
#include <qevent.h>

class DlgPerson : public QWidget
{
	Q_OBJECT
	public:
		DlgPerson(QWidget *parent, char *name);
		~DlgPerson();
	signals:
		void signalAccepted(int person);
	protected:
		void paintEvent(QPaintEvent *e);
	private slots:
		void slotPerson1();
		void slotPerson2();
		void slotPerson3();
};

#endif
