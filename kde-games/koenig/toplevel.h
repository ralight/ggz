// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _toplevel_h
#define _toplevel_h

#include <kmainwindow.h>
#include <qsocket.h>

class ChessBoardContainer;
class Options;
class Game;
class QMultiLineEdit;
class KListBox;
class KListView;
class KExtTabCtl;

class TopLevel : public KMainWindow
{
	Q_OBJECT
public:
	TopLevel(const char *name = 0);
	~TopLevel(void);
	void initGameSocket(void); // wrapper!
	void initLocal();

public slots:
	void newGame(void);
	void closeGame(void);
	//void handleNetInput(void);
	void slotTime(int timeoption, int time);
	void slotMessage(QString msg);
	void slotMove(QString msg);
	void slotDoMove(int x, int y, int x2, int y2);
	void slotStart(int seat);
	bool queryClose();

	void slotSync();
	void slotFlag();
	void slotDraw();
	void slotNetDraw();
	void slotWarnmessages();
	void slotBoardframe();
	void slotMoveTable();
	void slotChessBoard();

private:
	void initGameData(void);
	//void initGameSocket(void);

	ChessBoardContainer *chessBoard;
	Options *options;
	Game *game;
	QMultiLineEdit *tab1;
	KListBox *tab2;
	KListView *tab3;
	KExtTabCtl *ctl;

	bool warnings;
};

#endif
