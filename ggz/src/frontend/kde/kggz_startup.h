#include <qwidget.h>

class KGGZ_Startup : public QWidget
{
Q_OBJECT
public:
	KGGZ_Startup(QWidget *parent, char *name);
	~KGGZ_Startup();
	void setBackgroundPixmap(QPixmap pixmap);
};
