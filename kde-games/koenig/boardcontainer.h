#ifndef CHESS_BOARD_CONTAINER_H 
#define CHESS_BOARD_CONTAINER_H

#include <qwidget.h>
#include <qevent.h>

#include "board.h"

class ChessBoardContainer : public QWidget
{
	Q_OBJECT
	public:
		ChessBoardContainer(QWidget *parent = NULL, const char *name = NULL);
		~ChessBoardContainer();
		ChessBoard *root();

	protected:
		virtual void resizeEvent(QResizeEvent *e);

	private:
		ChessBoard *m_root;
};

#endif

