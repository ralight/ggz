#include <qwidget.h>
#include <qlineedit.h>

class KGGZ_Profiles : public QWidget
{
Q_OBJECT
public:
	KGGZ_Profiles(QWidget *parent, char *name);
	~KGGZ_Profiles();
protected slots:
	void accept();
	void slotRemoveProfile();
	void slotAddProfile();
private:
	QLineEdit *input_host, *input_port, *input_name;
};