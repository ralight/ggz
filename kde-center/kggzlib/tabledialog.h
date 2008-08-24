#ifndef TABLE_DIALOG_H
#define TABLE_DIALOG_H

// Qt includes
#include <QDialog>

#include "kggzlib_export.h"

class TableList;
class QPushButton;

namespace KGGZCore
{
	class Player;
}

class KGGZLIB_EXPORT TableDialog : public QDialog
{
	Q_OBJECT
	public:
		TableDialog(QWidget *parent = NULL);
		QList<KGGZCore::Player> seats();

	private slots:
		void slotManage();
		void slotUse();
		//void slotSelected(const GGZProfile& profile, int pos);

	private:
		void load();
		//void addProfile(const GGZProfile& profile);

		TableList *m_tablelist;
		QPushButton *m_use_button;
};

#endif

