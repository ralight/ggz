#ifndef KGGZ_PLAYERINFO_H
#define KGGZ_PLAYERINFO_H

#include <qwidget.h>
#include <qcheckbox.h>
#include <qlineedit.h>

class KGGZ_Playerinfo : public QWidget
{
Q_OBJECT
public:
	KGGZ_Playerinfo(QWidget *parent, char *name);
	~KGGZ_Playerinfo();
signals:
	void signalClosed();
protected slots:
	void slotAccept();
	void slotClose();
private:
	void readValues();
	void writeValues();

	QCheckBox *option_holdback;
	QLineEdit *input_name, *input_email, *input_homepage, *input_city, *input_country;
};

#endif
