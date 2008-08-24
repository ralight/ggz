#ifndef TABLE_DIALOG_H
#define TABLE_DIALOG_H

// Qt includes
#include <QDialog>

#include <kggzcore/gametype.h>
#include <kggzcore/table.h>

#include "kggzlib_export.h"

class TableList;
class QPushButton;

class KGGZLIB_EXPORT TableDialog : public QDialog
{
	Q_OBJECT
	public:
		TableDialog(QWidget *parent = NULL);
		void setGameType(const KGGZCore::GameType& gametype);
		void setIdentity(QString identity);

		KGGZCore::Table table() const;

	private slots:
		void slotManage();
		void slotUse();
		void slotSelected(const KGGZCore::Table& table, int pos);

	private:
		void load();
		//void addProfile(const GGZProfile& profile);

		TableList *m_tablelist;
		QPushButton *m_use_button;
		KGGZCore::GameType m_gametype;
		QString m_identity;
		QString m_description;
		KGGZCore::Table m_table;
		QList<bool> m_mods;
};

#endif

