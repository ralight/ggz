#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QtGui/QDialog>

#include <kggzcore/player.h>

class QScrollArea;
class QWidget;
class QLabel;
class QFrame;

class TableView : public QDialog
{
	Q_OBJECT
	public:
		TableView();
		void addPlayer(KGGZCore::Player player);
		void addSpectator(QString spectator);

	private slots:
		void slotDisplay(int id);

	private:
		void displaySeats();
		void displaySpectators();

		QScrollArea *m_view;
		QWidget *m_root;
		int m_oldmode;

		QList<KGGZCore::Player> m_players;
		QStringList m_spectators;

		enum DisplayModes
		{
			displayseats,
			displayspectators
		};
};

#endif
