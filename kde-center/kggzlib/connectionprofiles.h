#ifndef CONNECTION_PROFILES_H
#define CONNECTION_PROFILES_H

// Qt includes
#include <QDialog>

#include "kggzlib_export.h"

class ServerList;
class ConfigWidget;
class GGZProfile;

// The game window
class KGGZLIB_EXPORT ConnectionProfiles : public QDialog
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionProfiles(QWidget *parent = NULL);

		void setMetaserver(const QString &metaserver);

		QList<GGZProfile> profiles();

	private slots:
		void slotUpdate();
		void slotSelected(const GGZProfile& profile, int pos);
		void slotChanged(const GGZProfile& profile);
		void slotAccept();

	private:
		void load();
		void save(const GGZProfile& profile);
		void addProfile(const GGZProfile& profile);

		ServerList *m_serverlist;
		QString m_metaserver;
		ConfigWidget *m_configwidget;
		int m_pos;
};

#endif

