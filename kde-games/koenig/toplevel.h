#ifndef _toplevel_h
#define _toplevel_h

#include <kmainwindow.h>
#include <qsocket.h>

class ChessBoard;
class Options;
class Game;
class QMultiLineEdit;
class KListBox;

class TopLevel : public KMainWindow
{
	Q_OBJECT
public:
	TopLevel(const char *name = 0);
	~TopLevel(void);

public slots:
	void newGame(void);
	void closeGame(void);
	//void handleNetInput(void);
	void slotTime(int time);
	void slotMessage(QString msg);

private:
	void initGameData(void);
	void initGameSocket(void);

	ChessBoard *chessBoard;
	Options *options;
	Game *game;
	QMultiLineEdit *tab1;
	KListBox *tab2;
};

#endif
