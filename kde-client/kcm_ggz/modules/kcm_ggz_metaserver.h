#ifndef KCM_GGZ_METASERVER_H
#define KCM_GGZ_METASERVER_H

#include "kcm_ggz_pane.h"

class KCMGGZMetaserver : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZMetaserver(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZMetaserver();
		void load();
		void save();
		const char *caption();

	signals:
		void signalChanged();
};

#endif

