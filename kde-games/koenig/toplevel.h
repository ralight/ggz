#ifndef _toplevel_h
#define _toplevel_h

#include <kmainwindow.h>
#include <qsocket.h>

class ChessBoard;
class Options;

class TopLevel : public KMainWindow
{
	Q_OBJECT
public:
	TopLevel(const char *name = 0);
	~TopLevel(void);

public slots:
	void newGame(void);
	void closeGame(void);
	void handleNetInput(void);

private:
	void initGameData(void);
	void initGameSocket(void);

	ChessBoard *chessBoard;
	QSocket mainSocket;
	Options *options;
};

#endif
