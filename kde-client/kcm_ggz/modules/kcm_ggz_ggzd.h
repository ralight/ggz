#ifndef KCM_GGZ_GGZD_H
#define KCM_GGZ_GGZD_H

#include "kcm_ggz_pane.h"

class KListView;

class KCMGGZGgzd : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZGgzd(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZGgzd();
		void load();
		void save();
		const char *caption();

	signals:
		void signalChanged();

	private:
		void add(QString category, QString key);

		KListView *view;
};

#endif

