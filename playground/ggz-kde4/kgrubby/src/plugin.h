#ifndef KGRUBBY_PLUGIN_H
#define KGRUBBY_PLUGIN_H

#include <kdialog.h>

class Q3Table;
//class K3ListView;
//class QPushButton;

class Plugin : public KDialog
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
		Q3Table *m_settings;
};

#endif

