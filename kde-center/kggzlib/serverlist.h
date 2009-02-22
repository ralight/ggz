#ifndef SERVERLIST_H
#define SERVERLIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

#include "ggzprofile.h"

#include "kggzlib_export.h"

class QStandardItem;
class QStandardItemModel;
class QItemSelection;
class QListView;

// The game window
class KGGZLIB_EXPORT ServerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ServerList();
		void addProfile(const GGZProfile& profile);
		QList<GGZProfile> profiles();
		void clear();
		void updateProfile(const GGZProfile& profile, int pos);
		void removeProfile(int pos);
		void selectProfile(const GGZProfile& profile);

	signals:
		void signalSelected(const GGZProfile& profile, int pos);

	private slots:
		void slotLoaded(const QString& url, const QPixmap& pixmap);
		void slotActivated(const QItemSelection& index, const QItemSelection& prev);

	private:
		QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
		QList<GGZProfile> m_profiles;
		QList<GGZProfile*> m_profptrs;
		bool m_deletionmode;
		QListView *m_listview;
};

#endif

