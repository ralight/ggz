#ifndef KCM_GGZ_DEFAULT_H
#define KCM_GGZ_DEFAULT_H

#include "kcm_ggz_pane.h"

#include <qlineedit.h>

class KCMGGZDefault : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZDefault(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZDefault();
		void load();
		void save();
		const char *caption();

	signals:
		void signalChanged();

	private:
		QLineEdit *m_server, *m_username;
};

#endif

