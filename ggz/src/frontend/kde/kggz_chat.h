#include "kggz_chatwidget.h"
#include "kggz_tables.h"
#include "kggz_users.h"

class KGGZ_Chat : public QWidget
{
Q_OBJECT
public:
	KGGZ_Chat(QWidget *parent, char *name);
	~KGGZ_Chat();
private:
	KGGZ_Chatwidget *chatwidget;
	KGGZ_Tables *tables;
	KGGZ_Users *userlist;
};