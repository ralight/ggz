#include <qtextview.h>
#include <qlineedit.h>
#include <qevent.h>

class KGGZ_Chatwidget : public QWidget
{
Q_OBJECT
public:
	KGGZ_Chatwidget(QWidget *parent, char *name);
	~KGGZ_Chatwidget();
	static void receive(char *player, char *message);
	static void adminreceive(const char *adminmessage);
	static void inforeceive(const char *infomessage);
protected slots:
	void timerEvent(QTimerEvent *e);
	void send();
private:
	QTextView *output;
	QLineEdit *input;
};