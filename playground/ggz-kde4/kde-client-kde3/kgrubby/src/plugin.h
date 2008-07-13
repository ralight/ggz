#ifndef KGRUBBY_PLUGIN_H
#define KGRUBBY_PLUGIN_H

#include <kdialogbase.h>

class QTable;
//class KListView;
//class QPushButton;

class Plugin : public KDialogBase
{
	Q_OBJECT
	public:
		Plugin(QWidget *parent = NULL, const char *name = NULL);
		~Plugin();

		void addColumn(QString caption);

		void addRow(QStringList row);
		int numRows();
		QStringList getRow(int number);

	protected slots:
		void slotChanged(int row, int col);

	private:
		QTable *m_settings;
};

#endif

