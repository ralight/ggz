#ifndef SHADOW_CLIENT_GGZ_H
#define SHADOW_CLIENT_GGZ_H

#include <qwidget.h>
#include <kcombobox.h>
#include <qlist.h>

class ShadowClientGGZ : public QWidget
{
	Q_OBJECT
	public:
		ShadowClientGGZ(QWidget *parent = NULL, const char *name = NULL);
		~ShadowClientGGZ();
		void init();
	public slots:
		void slotPressed();
	signals:
		void signalClient(const char *name, const char *commandline);
	private:
		KComboBox *combo;
		QList<char> namelist, cmdlinelist;
};

#endif

