#ifndef KGGZ_LAUNCH_H
#define KGGZ_LAUNCH_H

#include <qwidget.h>
#include <qlistbox.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qlineedit.h>

class KGGZLaunch : public QWidget
{
	Q_OBJECT
	public:
		KGGZLaunch(QWidget *parent = NULL, char *name = NULL);
		~KGGZLaunch();

		const char *description();
		int seats();

	public slots:
		void slotSelected(QListBoxItem *selected, const QPoint&);
		void slotActivated(int id);
		void slotAccepted();
		void slotChanged(int value);

	signals:
		void signalLaunch();

	private:
		QListBox *m_listbox;
		QSlider *m_slider;
		QPopupMenu *m_popup;
		QLineEdit *m_edit;
};

#endif
