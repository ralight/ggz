#include <qpopupmenu.h>

class KGGZ_RoomsMenu : public QPopupMenu
{
Q_OBJECT
public:
	KGGZ_RoomsMenu(char *gamename, QWidget *parent, char *name);
	~KGGZ_RoomsMenu();
};
