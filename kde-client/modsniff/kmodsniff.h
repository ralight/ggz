#ifndef KMODSNIFF_H
#define KMODSNIFF_H

// Qt includes
#include <qwidget.h>

// Modsniff includes
#include "modsniff.h"

// Forward declarations
class QPushButton;
class QMultiLineEdit;

class KModSniff : public QWidget
{
	Q_OBJECT
	public:
		KModSniff(QWidget *parent = NULL, const char *name = NULL);
		~KModSniff();

	public slots:
		void slotSearch();

	private:
		void addModule(const char *modulename, const char *frontend);
		GGZModuleEntry *installedModules();

		QMultiLineEdit *m_ed;
		QPushButton *m_search;
};

#endif

