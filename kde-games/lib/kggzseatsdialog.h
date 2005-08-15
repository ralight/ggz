#ifndef KGGZ_SEATSDIALOG_H
#define KGGZ_SEATSDIALOG_H

#include <qwidget.h>

#include <ggzmod.h>

namespace KIO
{
	class Job;
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

		void setMod(GGZMod *mod);

	private slots:
		void slotDisplay(int id);
		void slotTaskData(KIO::Job *job, const QByteArray&);
		void slotTaskResult(KIO::Job *job);

	private:
		GGZMod *m_mod;
		QScrollView *m_view;
		QWidget *m_root;
		QMap<int, QLabel*> m_hostnames;
		QMap<int, QLabel*> m_realnames;
		QMap<int, QFrame*> m_photos;
		QMap<KIO::Job*, int> m_phototasks;
		QMap<KIO::Job*, QByteArray> m_photodata;

		enum DisplayModes
		{
			displayseats,
			displayspectators
		};

		void displaySeats();
		void displaySpectators();
		void infos();
		static void handle_info(GGZMod *mod, GGZModEvent e, const void *data);
};

#endif

