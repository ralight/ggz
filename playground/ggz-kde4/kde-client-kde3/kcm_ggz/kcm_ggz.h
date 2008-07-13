#ifndef KCM_GGZ_H
#define KCM_GGZ_H

#include <kcmodule.h>
#include <qptrlist.h>

class KCMGGZPane;

class KCMGGZ : public KCModule
{
	Q_OBJECT
	public:
		KCMGGZ(KInstance *instance = NULL, QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZ();
		void load();
		void save();
		void defaults();

		QString quickHelp();
		const KAboutData *aboutData();

	public slots:
		void slotChanged();

	private:
		QPtrList<KCMGGZPane> panelist;
};

#endif

