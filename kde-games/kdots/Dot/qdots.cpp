///////////////////////////////////////////////////////////////
//
// Dots
// C++ Class Set for Connect the Dots games
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "qdots.h"

#include <qimage.h>
#include <qpainter.h>
#include <qpen.h>
#include <qarray.h>

#include <iostream>
#include <stdio.h>

#include "config.h"

QPixmap *bgpix;
QImage *bgimg, *fgimg;
QArray<int> *m_shadow;
int m_arywidth, m_aryheight;

QDots::QDots(QWidget* parent = 0, char* name = 0)
: QWidget(parent, name), VDots()
{
	bgpix = NULL;
	fgimg = NULL;
	bgimg = NULL;
	m_shadow = NULL;

	//setBackgroundColor(Qt::blue);
	//setBackgroundPixmap(QPixmap(GGZDATADIR "/kdots/dragon.png"));
	bgpix = new QPixmap(GGZDATADIR "/kdots/dragon.png");
	grayscale(bgpix);
	bgimg = new QImage(bgpix->convertToImage());
	fgimg = new QImage(bgpix->convertToImage());
	setBackgroundPixmap(*bgpix);
}

QDots::~QDots()
{
	if(bgpix) delete bgpix;
	if(bgimg) delete bgimg;
	if(fgimg) delete fgimg;
	if(m_shadow) delete m_shadow;
}

QPixmap *QDots::grayscale(QPixmap *pix)
{
	QPainter p;
	QImage *tmp;
	QRgb rgb;

	tmp = new QImage(bgpix->convertToImage());

	for(int j = 0; j < pix->height(); j++)
		for(int i = 0; i < pix->width(); i++)
		{
			rgb = tmp->pixel(i, j);
			tmp->setPixel(i, j, qRgb(qBlue(rgb), qBlue(rgb), qBlue(rgb)));
		}

	p.begin(pix);
	p.drawImage(0, 0, *tmp, 0, 0, pix->width(), pix->height());
	p.end();

	delete tmp;
}

void QDots::resizeEvent(QResizeEvent *e)
{
	int w, h;

	if((m_rows < 0) || (m_cols < 0)) return;

	if(e)
	{
		w = e->size().width();
		h = e->size().height();
	}
	else
	{
		w = width();
		h = height();
	}

	m_xoffset = w / (m_cols + 1);
	m_yoffset = h / (m_rows + 1);

	w -= m_xoffset * 2;
	h -= m_yoffset * 2;

	vResizeBoard(w, h);

	paintEvent(NULL);
}

void QDots::paintEvent(QPaintEvent *e)
{
	int tilewidth, tileheight;
	QRgb rgb;
	int bluepart;
	QPainter p;
	int w, h;
	int ws, hs;

	if((m_rows < 0) || (m_cols < 0)) return;
	if(!m_shadow) return;

	tileheight = height() / (m_aryheight + 2);
	tilewidth = width() / (m_arywidth + 2);

	w = width();
	h = height();
	ws = w / (m_cols + 1);
	hs = h / (m_rows + 1);

	for(int j = 1; j < m_rows; j++)
	{
		for(int i = 1; i < m_cols; i++)
		{
			if(m_shadow->at((j - 1) * m_arywidth + i - 1) != content(i - 1, j - 1))
			{
				m_shadow->at((j - 1) * m_arywidth + i - 1) = content(i - 1, j - 1);
				for(int y = 0; y < tileheight; y++)
					for(int x = 0; x < tilewidth; x++)
					{
						rgb = bgimg->pixel(i * tilewidth + x, j * tileheight + y);
						bluepart = qBlue(rgb) / 2;
						if(bluepart < 50) bluepart = 50;
						if(content(i - 1, j - 1) == 1) bluepart *= 3;
						if(bluepart > 255) bluepart = 255;
						fgimg->setPixel(i * tilewidth + x, j * tileheight + y, qRgb(0, 0, bluepart));
					}
				p.begin(bgpix);
				p.drawImage(0, 0, *fgimg, 0, 0, width(), height());
				p.end();
				setBackgroundPixmap(*bgpix);
			}
		}
	}

	p.begin(this);

	for(int j = 1; j <= m_rows; j++)
	{
		for(int i = 1; i <= m_cols; i++)
		{

			if(i < m_cols)
			{
				if(border(i - 1, j - 1, right) != -1) p.setPen(Qt::darkBlue);
				else p.setPen(Qt::yellow);
				p.drawLine(i * ws, j * hs, (i + 1) * ws, j * hs);
				p.drawLine(i * ws, j * hs + 1, (i + 1) * ws, j * hs + 1);
			}

			if(j < m_rows)
			{
				if(border(i - 1, j - 1, down) != -1) p.setPen(Qt::darkBlue);
				else p.setPen(Qt::yellow);
				p.drawLine(i * ws, j * hs, i * ws, (j + 1) * hs);
				p.drawLine(i * ws + 1, j * hs, i * ws + 1, (j + 1) * hs);
			}

			p.setPen(QPen(Qt::white));
			p.drawPoint(i * ws, j * hs);
			p.drawPoint(i * ws + 1, j * hs);
			p.drawPoint(i * ws, j * hs + 1);
			p.drawPoint(i * ws + 1, j * hs + 1);
		}
	}

	p.end();
}

void QDots::mousePressEvent(QMouseEvent *e)
{
	int x, y;
	int success;

	x = e->x();
	y = e->y();

	x -= m_xoffset;
	y -= m_yoffset;

	// These two lines are essential for GGZ mode!!!
	success = vSetBorderValue(x, y, 1);
	if(success) emit signalTurn(m_lastx, m_lasty, m_lastdirection);
}

// Reinitializes shadow structure
void QDots::refreshBoard()
{
	QPainter p;

	if(m_shadow) delete m_shadow;

	m_shadow = new QArray<int>((m_cols - 1) * (m_rows - 1));
	for(int j = 0; j < m_rows -1; j++)
		for(int i = 0; i < m_cols - 1; i++)
			m_shadow->at(j * (m_cols - 1) + i) = -1;
	m_arywidth = m_cols - 1;
	m_aryheight = m_rows - 1;

	p.begin(bgpix);
	p.drawImage(0, 0, *bgimg, 0, 0, width(), height());
	p.end();
	if(fgimg) delete fgimg;
	fgimg = new QImage(bgpix->convertToImage());
	setBackgroundPixmap(*bgpix);

	resizeEvent(NULL);
	repaint();
}
