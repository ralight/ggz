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
#include <config.h>
#include "reversiview.h"
#include "tiles.xpm"

/* Reversi Disc */
ReversiDisc::ReversiDisc( QCanvasPixmapArray *seq, QCanvas *canv ) : QCanvasSprite(seq, canv) {
}

ReversiDisc::~ReversiDisc() {
}

void ReversiDisc::advance( int stage ) {
  int a = frame();
  if (stage == 0)
    return;
  if (a < 10) {
    setFrame(a+1);
    if (a == 9)
      setAnimated(false);
  } else if (a >= 10 && a < 19) {
    setFrame(a+1);
    if (a == 18)
      setAnimated(false);
  } else if (a == 19) {
    setFrame(0);
  }
}

/* Reversi View */

ReversiView::ReversiView(QWidget * parent, const char * name, WFlags f) : QCanvasView(0, parent, name, f) {
	canvas = new QCanvas( QPixmap::QPixmap(tiles_xpm), 8, 8, 50, 50 );
  /*
	canvas->setTile( 3, 3, 1 );
	canvas->setTile( 4, 4, 1 );
	canvas->setTile( 3, 4, 2 );
	canvas->setTile( 4, 3, 2 );
  */
	setCanvas(canvas);
  
//  QString theme = GGZDATADIR;
//  theme += "/kreversi/pixmaps/default/disc0000.png";

  disc_img = new QCanvasPixmapArray( GGZDATADIR "/kreversi/pixmaps/default/disc%1.png", 20 );
//  debug(theme);

  /* Add the initial four discs */
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 3*50, 3*50, 10 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 4*50, 4*50, 10 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 3*50, 4*50, 0 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 4*50, 3*50, 0 );
  discs.last()->show();

  canvas->setAdvancePeriod( 100 );


}

void ReversiView::updateBoard(char board[8][8]) {
  int x, y;
  QCanvasSprite *a;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      printf("%d ", board[x][y]);
    }
    printf("\n");
  }
  for (a = discs.first(); a; a = discs.next()) {
    a->hide();
    a->setAnimated(false);
  }
  a = discs.first();
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      if (board[x][y] == OPEN)
        continue;
      if (!a) {
        discs.append( new ReversiDisc(disc_img, canvas) );
        a = discs.last();
      }
      if (board[x][y] == WHITE) {
        a->move(x*50, y*50, 10);
        a->show();
      } else if (board[x][y] == BLACK) {
        a->move(x*50, y*50, 0);
        a->show();
      } else if (board[x][y] == 2*WHITE) {
        a->move(x*50, y*50, 0);
        a->show();
        a->setAnimated(true);
      } else if (board[x][y] == 2*BLACK) {
        a->move(x*50, y*50, 10);
        a->show();
        a->setAnimated(true);
      }
      a = discs.next();
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
