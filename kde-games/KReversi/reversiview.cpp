/***************************************************************************
                          reversiview.cpp  -  description
                             -------------------
    begin                : Fri Mar 9 2001
    copyright            : (C) 2001 by Ismael Orenstein
    email                : perdig@linuxbr.com.br
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include "reversiview.h"
#include "tiles.xpm"

ReversiView::ReversiView(QWidget * parent, const char * name, WFlags f) : QCanvasView(0, parent, name, f) {
	canvas = new QCanvas( QPixmap::QPixmap(tiles_xpm), 8, 8, 50, 50 );
	canvas->setTile( 3, 3, 1 );
	canvas->setTile( 4, 4, 1 );
	canvas->setTile( 3, 4, 2 );
	canvas->setTile( 4, 3, 2 );
	setCanvas(canvas);

}

void ReversiView::updateBoard(char board[8][8]) {
  int x, y;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      printf("%d ", board[x][y]);
    }
    printf("\n");
  }
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      if (board[x][y] == WHITE) {
        canvas->setTile(x, y, 1);
      } else if (board[x][y] == BLACK) {
        canvas->setTile(x, y, 2);
      } else {
        canvas->setTile(x, y, 0);
      }
    }
  }
  canvas->update();
}

ReversiView::~ReversiView(){
}

void ReversiView::contentsMousePressEvent(QMouseEvent *e){
  int x, y;
  x = e->x() / 50;
  y = e->y() / 50;
  emit playerMove(x, y);
}
