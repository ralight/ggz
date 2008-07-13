#ifndef KCM_GGZ_DRS_H
#define KCM_GGZ_DRS_H

#include "kcm_ggz_pane.h"

class KCMGGZDRS : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZDRS(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZDRS();
		void load();
		void save();
		QString caption();

	signals:
		void signalChanged();
};

#endif

