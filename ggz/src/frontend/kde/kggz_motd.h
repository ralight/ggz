#include <qwidget.h>
#include <qtimer.h>

class KGGZ_Motd : public QWidget
{
Q_OBJECT
public:
	KGGZ_Motd(QWidget *parent, char *name);
	~KGGZ_Motd();
	static void append(char *text);
	void timerEvent(QTimerEvent *e);
};