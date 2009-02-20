#ifndef MODULE_DIALOG_H
#define MODULE_DIALOG_H

#include <QDialog>

#include "kggzlib_export.h"

class QTableView;
class QRecursiveSortFilterProxyModel;
class QStandardItemModel;

class KGGZLIB_EXPORT ModuleDialog : public QDialog
{
	Q_OBJECT
	public:
		ModuleDialog(QWidget *parent = NULL);

	private slots:
		void slotSearch(const QString& text);

	private:
		void load();

		QTableView *m_modules;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QStandardItemModel *m_model;
};

#endif

