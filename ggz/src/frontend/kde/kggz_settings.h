#include <qwidget.h>

class KGGZ_Settings : public QWidget
{
Q_OBJECT
public:
	KGGZ_Settings(QWidget *parent, char *name);
	~KGGZ_Settings();
protected slots:
	void accept();
};