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
#include "config.h"
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>
#include "reversiview.h"

/* Reversi Disc */
ReversiDisc::ReversiDisc( QCanvasPixmapArray *seq, QCanvas *canv ) : QCanvasSprite(seq, canv) {
  dir = 0;
}

ReversiDisc::~ReversiDisc() {
}

void ReversiDisc::advance( int stage ) {
  int a = frame();
  if (stage == 0)
    return;
  if (a == 0)
    dir = 1;
  if (a == frameCount()-1)
    dir = -1;
  setFrame(a+dir);
  if (a+dir == 0 || a+dir == frameCount()-1)
    setAnimated(false);
}

/* Reversi View */

ReversiView::ReversiView(QString theme, QWidget * parent, const char * name, WFlags f) : QCanvasView(0, parent, name, f) {
  
  /* Create canvas */
  canvas = new QCanvas( 50*8, 50*8 );
	setCanvas(canvas);
  
  /* Load pixmaps */
  disc_img = NULL;
  if ( loadTheme( theme, false ) < 0 ) {
    //kapp->closeAllWindows();
    //kapp->quit();
    //delete this;
    //exit(-1);
    return;
  }
//  debug(theme);

  /* Add the initial four discs */
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 3*50, 3*50, discs.last()->frameCount()-1 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 4*50, 4*50, discs.last()->frameCount()-1 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 3*50, 4*50, 0 );
  discs.last()->show();
  discs.append( new ReversiDisc(disc_img, canvas) );
  discs.last()->move( 4*50, 3*50, 0 );
  discs.last()->show();

}

int ReversiView::loadTheme( QString theme, bool keep ) {
  ReversiDisc *d;
  int board[8][8];
  KSimpleConfig conf(GGZDATADIR "/kreversi/pixmaps/" + theme + "/themerc", true);
  int x, y;
  int frames, speed;

  /* Animation settings */
  conf.setGroup("Animation");
  frames = conf.readNumEntry("Frames", 20);
  speed = conf.readNumEntry("Speed", 50);
  
  /* Load images */
  conf.setGroup("Filenames");
  QPixmap tile_img(GGZDATADIR "/kreversi/pixmaps/" + theme + "/" + conf.readEntry("Tiles", "tiles.png"));

  if (tile_img.isNull()) {
    /* Couldn't load theme! */
    KMessageBox::error(this, i18n("Couldn't load theme " + theme +"! Check your installation!"));
    return -1;
  }

  disc_img = new QCanvasPixmapArray( GGZDATADIR "/kreversi/pixmaps/" + theme + "/" + conf.readEntry("Disc", "disc%1.png") , frames);

  /* Names */
  conf.setGroup("Theme");
  colorNames.clear();
  colorNames.append( conf.readEntry("Player1", "blue" ) );
  colorNames.append( conf.readEntry("Player2", "red" ) );

  /* Load tiles */
  
  if (keep) {
    for (x = 0; x < 8; x++) {
      for (y = 0; y < 8; y++) {
        board[x][y] = canvas->tile(x, y);
      }
    }
  }

  /* Set images */
  canvas->setTiles( tile_img, 8, 8, 50, 50 );
  if (keep) {
    for (x = 0; x < 8; x++) {
      for (y = 0; y < 8; y++) {
        canvas->setTile( x, y, board[x][y] );
      }
    }
  }

  for (d = discs.first(); d; d = discs.next()) {
    x = d->frame();
    d->setFrame( 0 );
    d->setSequence( disc_img );
    d->setFrame( x==0?0:frames-1 );
  }

  canvas->update();
  canvas->setAdvancePeriod(speed);

  return 0;

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
      if (board[x][y] < 8 && !a) {
        discs.append( new ReversiDisc(disc_img, canvas) );
        a = discs.last();
      }
      /* Clean up tile */
      canvas->setTile(x, y, 0);
      switch(board[x][y]) {
        case VIEW_OPEN:
          canvas->setTile(x, y, 0);
          break;
        case VIEW_POSSIBLE:
          canvas->setTile(x, y, 2);
          break;
        case VIEW_LAST_WHITE:
          canvas->setTile(x, y, 1);
        case VIEW_WHITE:
          a->move(x*50, y*50, a->frameCount()-1);
          a->show();
          break;
        case VIEW_LAST_BLACK:
          canvas->setTile(x, y, 1);
        case VIEW_BLACK:
          a->move(x*50, y*50, 0);
          a->show();
          break;
        case VIEW_MOVE_WHITE:
          a->move(x*50, y*50, 0);
          a->show();
          a->setAnimated(true);
          break;
        case VIEW_MOVE_BLACK:
          a->move(x*50, y*50, a->frameCount()-1);
          a->show();
          a->setAnimated(true);
          break;
      }
      if (a)
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
