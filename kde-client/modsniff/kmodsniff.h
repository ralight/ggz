#ifndef KMODSNIFF_H
#define KMODSNIFF_H

#include <qwidget.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>

class KModSniff : public QWidget
{
	Q_OBJECT
	public:
		KModSniff(QWidget *parent = NULL, char *name = NULL);
		~KModSniff();

	public slots:
		void slotSearch();

	private:
		void addModule(char *modulename);
		char **installedModules();

		QMultiLineEdit *m_ed;
		QPushButton *m_search;
};

#endif

