#include <qwidget.h>

class KGGZ_Launch : public QWidget
{
Q_OBJECT
public:
	KGGZ_Launch(QWidget *parent, char *name);
	~KGGZ_Launch();
protected slots:
	void accept();
};