#ifndef DLG_HELP_H
#define DLG_HELP_H

#include <qwidget.h>
#include <qevent.h>

class DlgHelp : public QWidget
{
	Q_OBJECT
	public:
		DlgHelp(QWidget *parent, char *name);
		~DlgHelp();
	protected:
		void paintEvent(QPaintEvent *e);
};

#endif
