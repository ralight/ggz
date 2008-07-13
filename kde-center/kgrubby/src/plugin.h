#ifndef KGRUBBY_PLUGIN_H
#define KGRUBBY_PLUGIN_H

#include <kdialog.h>

class QTableWidget;

class Plugin : public KDialog
{
	Q_OBJECT
	public:
		Plugin(QWidget *parent = NULL);
		~Plugin();

		void addColumn(QString caption);

		void addRow(QStringList row);
		int numRows();
		QStringList getRow(int number);

	protected slots:
		void slotChanged(int row, int col);

	private:
		QTableWidget *m_settings;
};

#endif

