#ifndef _toplevel_h
#define _toplevel_h

#include <kmainwindow.h>
#include <qsocket.h>

class ChessBoardContainer;
class Options;
class Game;
class QMultiLineEdit;
class KListBox;
class KExtTabCtl;

class TopLevel : public KMainWindow
{
	Q_OBJECT
public:
	TopLevel(const char *name = 0);
	~TopLevel(void);
	void initGameSocket(void); // wrapper!

public slots:
	void newGame(void);
	void closeGame(void);
	//void handleNetInput(void);
	void slotTime(int timeoption, int time);
	void slotMessage(QString msg);
	void slotMove(QString msg);
	void slotDoMove(int x, int y, int x2, int y2);
	void slotStart(int seat);

	void slotSync();
	void slotFlag();
	void slotDraw();
	void slotNetDraw();

private:
	void initGameData(void);
	//void initGameSocket(void);

	ChessBoardContainer *chessBoard;
	Options *options;
	Game *game;
	QMultiLineEdit *tab1;
	KListBox *tab2;
	KExtTabCtl *ctl;
};

#endif
