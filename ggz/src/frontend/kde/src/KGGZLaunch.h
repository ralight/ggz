#ifndef KGGZ_LAUNCH_H
#define KGGZ_LAUNCH_H

#include <qwidget.h>
#include <qlistview.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include <qcstring.h>
#include <qpushbutton.h>
#include <qlabel.h>

class KGGZLaunch : public QWidget
{
	Q_OBJECT
	public:
		KGGZLaunch(QWidget *parent = NULL, char *name = NULL);
		~KGGZLaunch();

		void initLauncher(char *playername, int maxplayers);
		const char *description();
		int seats();
		int seatType(int seat);

		enum SeatTypes
		{
			seatopen = -1,
			seatreserved = -3,
			seatbot = -2,
			seatplayer = -5,
			seatunused = -4,
			seatunknown = -6
		};

	public slots:
		void slotSelected(QListViewItem *selected, const QPoint&, int column);
		void slotActivated(int id);
		void slotAccepted();
		void slotChanged(int value);

	signals:
		void signalLaunch();

	private:
		void setSeatType(int seat, int seattype);
		const char *typeName(int seattype);
		
		QListView *m_listbox;
		QSlider *m_slider;
		QPopupMenu *m_popup;
		QLineEdit *m_edit;
		char *m_playername;
		QByteArray *m_array;
		QPushButton *m_ok;
		QLabel *m_label;
};

#endif
