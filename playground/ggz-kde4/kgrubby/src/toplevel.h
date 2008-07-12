#ifndef KGRUBBY_TOPLEVEL_H
#define KGRUBBY_TOPLEVEL_H

#include <kmainwindow.h>

class App;
class KMenu;

class Toplevel : public KMainWindow
{
	Q_OBJECT
	public:
		Toplevel();
		~Toplevel();

	protected slots:
		void slotMenu(int id);
		void slotChanged(bool changed);

	private:
		enum MenuItems
		{
			menusave,
			menuquit
		};

		App *m_app;
		KMenu *menu_file;

};

#endif

