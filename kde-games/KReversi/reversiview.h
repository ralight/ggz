/***************************************************************************
                          reversiview.h  -  description
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

#ifndef REVERSIVIEW_H
#define REVERSIVIEW_H

#include <qcanvas.h>

#define WHITE +1
#define BLACK -1
#define OPEN 0

/**Little disc
  *@author Ismael Orenstein
  */

class ReversiDisc : public QCanvasSprite {

public:
  ReversiDisc( QCanvasPixmapArray *, QCanvas *);
  ~ReversiDisc();
  virtual void advance( int stage );

};

/**Abstracts the game view
  *@author Ismael Orenstein
  */

class ReversiView : public QCanvasView {
  Q_OBJECT
public: 
	ReversiView(QWidget * parent=0, const char * name=0, WFlags f=0);
	~ReversiView();
signals:
  void playerMove(int x, int y);
public:
  void updateBoard(char board[8][8]);
private: // Private attributes
  /** Should be used by a QCanvasView to display the game */
  QCanvas *canvas;
protected: // Protected methods
  /** Used when the mouse is pressed */
  void contentsMousePressEvent(QMouseEvent *e);
  /** List of discs */
  QList<ReversiDisc> discs;
  /** discs image */
  QCanvasPixmapArray *disc_img;
};

#endif
