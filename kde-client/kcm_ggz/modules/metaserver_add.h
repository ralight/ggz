#ifndef METASERVER_ADD_H
#define METASERVER_ADD_H

#include <qwidget.h>
#include <qlineedit.h>

class MetaserverAdd : public QWidget
{
	Q_OBJECT
	public:
		MetaserverAdd(QWidget *parent = NULL, const char *name = NULL);
		~MetaserverAdd();
	public slots:
		void slotAccept();
	signals:
		void signalAdd(QString uri, QString proto);
	private:
		QLineEdit *eduri, *edproto;
};

#endif

