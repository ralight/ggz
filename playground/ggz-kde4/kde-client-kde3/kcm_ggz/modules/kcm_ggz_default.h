#ifndef KCM_GGZ_DEFAULT_H
#define KCM_GGZ_DEFAULT_H

#include "kcm_ggz_pane.h"

class QPushButton;
class QLineEdit;

class KCMGGZDefault : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZDefault(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZDefault();
		void load();
		void save();
		QString caption();

	public slots:
		void slotPersonalization();

	signals:
		void signalChanged();

	private:
		void loadPicture();

		QLineEdit *m_server, *m_username;
		QPushButton *m_button_personalization;
		QString m_picture;
};

#endif

