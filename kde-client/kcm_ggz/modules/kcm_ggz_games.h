#ifndef KCM_GGZ_GAMES_H
#define KCM_GGZ_GAMES_H

#include "kcm_ggz_pane.h"

class KListView;
class QListViewItem;

class KCMGGZGames : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZGames(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZGames();
		void load();
		void save();
		const char *caption();

	public slots:
		void slotSelected(QListViewItem *item, const QPoint &point, int column);

	signals:
		void signalChanged();

	private:
		void add(QString location, QString name, QString frontend, QString authors, QString homepage, QString version, QString protocol);

		KListView *view;
};

#endif

