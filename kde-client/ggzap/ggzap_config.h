#ifndef GGZAP_CONFIG_H
#define GGZAP_CONFIG_H

#include <qwidget.h>
#include <qlineedit.h>

class GGZapConfig : public QWidget
{
	Q_OBJECT
	public:
		GGZapConfig(QWidget *parent = NULL, const char *name = NULL);
		~GGZapConfig();

	public slots:
		void slotAccept();

	private:
		void configSave();
		void configLoad();
	
		QLineEdit *m_server, *m_username;
		QString *configfile;
};

#endif

