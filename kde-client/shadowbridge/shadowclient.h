#ifndef SHADOW_CLIENT_H
#define SHADOW_CLIENT_H

#include <qwidget.h>
#include <klineedit.h>

class ShadowClient : public QWidget
{
	Q_OBJECT
	public:
		ShadowClient(QWidget *parent = NULL, const char *name = NULL);
		~ShadowClient();
	public slots:
		void slotPressed();
	signals:
		void signalClient(const char *name, const char *commandline);
	private:
		KLineEdit *sockname, *input;
};

#endif

