#ifndef MODULE_DIALOG_H
#define MODULE_DIALOG_H

#include <QDialog>

#include "kggzlib_export.h"

class QTableWidget;

class KGGZLIB_EXPORT ModuleDialog : public QDialog
{
	Q_OBJECT
	public:
		ModuleDialog(QWidget *parent = NULL);

	private:
		void load();

		QTableWidget *m_modules;
};

#endif

