#ifndef KCM_GGZ_GAMES_H
#define KCM_GGZ_GAMES_H

#include "kcm_ggz_pane.h"

class KListView;
class QListViewItem;
class QPopupMenu;

class KCMGGZGames : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZGames(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZGames();
		void load();
		void save();
		QString caption();
		enum MenuEntries
		{
			menuhomepage,
			menuinformation
		};

	public slots:
		void slotSelected(QListViewItem *item, const QPoint& point, int column);
		void slotUpdate();
		void slotActivated(int index);

	signals:
		void signalChanged();

	private:
		void add(QString location, QString name, QString frontend, QString authors, QString homepage, QString version, QString protocol);

		KListView *view;
		QPopupMenu *popup;
};

#endif

