#ifndef METASERVER_ADD_SERVER_H
#define METASERVER_ADD_SERVER_H

#include <kdialogbase.h>
#include <qlineedit.h>

class MetaserverAddServer : public KDialogBase
{
	Q_OBJECT
	public:
		MetaserverAddServer(QWidget *parent = NULL, const char *name = NULL);
		~MetaserverAddServer();
	public slots:
		void slotAccept();
	signals:
		void signalAdd(QString uri, QString type, QString location, QString speed, QString comment);
	private:
		QLineEdit *eduri, *edtype, *edcomment, *edspeed, *edlocation;
};

#endif

