#ifndef KGGZ_SETTINGS_H
#define KGGZ_SETTINGS_H

#include <qwidget.h>
#include <qcheckbox.h>

class KGGZ_Settings : public QWidget
{
Q_OBJECT
public:
	KGGZ_Settings(QWidget *parent, char *name);
	~KGGZ_Settings();
signals:
	void signalClosed();
protected slots:
	void slotAccept();
	void slotClose();
private:
	void readValues();
	void writeValues();

	QCheckBox *option_chatlog, *option_session, *option_feature, *option_startup, *option_java;
};

#endif