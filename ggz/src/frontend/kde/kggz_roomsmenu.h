#include <kpopupmenu.h>

class KGGZ_RoomsMenu : public KPopupMenu
{
Q_OBJECT
public:
	KGGZ_RoomsMenu(char *gamename, QWidget *parent, char *name);
	~KGGZ_RoomsMenu();
	static void add(char *name);
	static void removeAll();
public slots:
	static void handleMenu(int id);
};
