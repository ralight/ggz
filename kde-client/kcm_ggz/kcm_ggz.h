#ifndef KCM_GGZ_H
#define KCM_GGZ_H

#include <kcmodule.h>
#include <qlist.h>

class KCMGGZPane;

class KCMGGZ : public KCModule
{
	Q_OBJECT
	public:
		KCMGGZ(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZ();
		void load();
		void save();
		void defaults();
		QString quickHelp();

	public slots:
		void slotChanged();

	private:
		QList<KCMGGZPane> panelist;
};

#endif

