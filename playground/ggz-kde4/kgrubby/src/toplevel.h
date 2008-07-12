#ifndef KGRUBBY_TOPLEVEL_H
#define KGRUBBY_TOPLEVEL_H

#include <kmainwindow.h>

class App;
class KMenu;
class QAction;

class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();

	protected slots:
		void slotMenu(QAction *action);
		void slotChanged(bool changed);

	private:
		App *m_app;
		KMenu *menu_file;
		QAction *action_save, *action_quit;

};

#endif

