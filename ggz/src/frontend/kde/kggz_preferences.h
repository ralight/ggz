#include <qwidget.h>
#include <qlistview.h>

class KGGZ_Preferences : public QWidget
{
Q_OBJECT
public:
	KGGZ_Preferences(QWidget *parent, char *name);
	~KGGZ_Preferences();
protected slots:
	void slotAction(QListViewItem *item, const QPoint &point, int column);
	void slotDefault();
};