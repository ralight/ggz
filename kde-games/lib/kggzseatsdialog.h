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
};

class QScrollView;
class QLabel;
class QFrame;

class KGGZSeatsDialog : public QWidget
{
	Q_OBJECT
	public:
		KGGZSeatsDialog(QWidget *parent = NULL, const char *name = NULL);
		~KGGZSeatsDialog();

		void setMod(KGGZMod::Module *mod);

	private slots:
		void slotDisplay(int id);
		void slotTaskData(KIO::Job *job, const QByteArray&);
		void slotTaskResult(KIO::Job *job);
		void slotInfo(KGGZMod::Event event);

	private:
		KGGZMod::Module *m_mod;
		QScrollView *m_view;
		QWidget *m_root;
		QMap<int, QLabel*> m_hostnames;
		QMap<int, QLabel*> m_realnames;
		QMap<int, QFrame*> m_photos;
		QMap<KIO::Job*, int> m_phototasks;
		QMap<KIO::Job*, QByteArray> m_photodata;
		int m_oldmode;

		enum DisplayModes
		{
			displayseats,
			displayspectators
		};

		void displaySeats();
		void displaySpectators();
		void infos();
};

#endif

