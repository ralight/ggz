#ifndef CONNECTION_SINGLE_H
#define CONNECTION_SINGLE_H

// Qt includes
#include <QDialog>

#include "kggzlib_export.h"

class ServerList;
class ConfigWidget;
class GGZProfile;

class KGGZLIB_EXPORT ConnectionSingle : public QDialog
{
	Q_OBJECT
	public:
		ConnectionSingle(QWidget *parent = NULL);

		GGZProfile profile();

		void presetServer(const QString& uri);

	private slots:
		void slotChanged(const GGZProfile& profile);
		void slotAccept();

	private:
		void addProfile(const GGZProfile& profile, bool selected);

		ServerList *m_serverlist;
		ConfigWidget *m_configwidget;
};

#endif

