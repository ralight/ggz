#ifndef KGGZGAMES_SEATSDIALOG_H
#define KGGZGAMES_SEATSDIALOG_H

#include <kggzmod/event.h>

#include <qwidget.h>

namespace KIO
{
	class Job;
};

namespace KGGZMod
{
	class Module;
	class Player;
};

class QScrollArea;
class QLabel;
class QFrame;
class QToolButton;
class QAction;

class KGGZSeatsDialog : public QWidget
{
	Q_OBJECT
	public:
		KGGZSeatsDialog(QWidget *parent = NULL);
		~KGGZSeatsDialog();

		void setMod(KGGZMod::Module *mod);

	private slots:
		void slotDisplay(int id);
		void slotTaskData(KIO::Job *job, const QByteArray&);
		void slotTaskResult(KIO::Job *job);
		void slotInfo(const KGGZMod::Event& event);
		void slotAction();
		void slotMenu(QAction *action);

	private:
		KGGZMod::Module *m_mod;
		QScrollArea *m_view;
		QWidget *m_root;
		QMap<int, QLabel*> m_hostnames;
		QMap<int, QLabel*> m_realnames;
		QMap<int, QFrame*> m_photos;
		QMap<KIO::Job*, int> m_phototasks;
		QMap<KIO::Job*, QByteArray> m_photodata;
		QMap<const QObject*, int> m_buttons;
		QMap<const QObject*, QToolButton*> m_buttondata;
		int m_oldmode;
		KGGZMod::Player *m_currentplayer;

		enum DisplayModes
		{
			displayseats,
			displayspectators
		};

		QAction *action_standup;
		QAction *action_sitdown;
		QAction *action_bootplayer;
		QAction *action_botadd;
		QAction *action_botremove;
		QAction *action_viewstats;

		void displaySeats();
		void displaySpectators();
		void infos();
};

#endif

