#include <qwidget.h>

class KGGZ_Connect : public QWidget
{
Q_OBJECT
public:
	KGGZ_Connect(QWidget *parent, char *name);
	~KGGZ_Connect();
	static void success(int connected);
protected slots:
	void accept();
	void edit();
};