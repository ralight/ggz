///////////////////////////////////////////////////////////////
//
// Dots
// C++ Class Set for Connect the Dots games
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "qdots.h"

#include <qimage.h>
#include <qpainter.h>
#include <qpen.h>
#include <qmemarray.h>

#include "config.h"

QPixmap *bgpix;
QImage *bgimg, *fgimg;
QMemArray<int> *m_shadow;
QMemArray<int> *m_shadowlines;
int m_arywidth, m_aryheight;

QDots::QDots(QWidget* parent, const char* name)
: QWidget(parent, name), VDots()
{
	bgpix = NULL;
	fgimg = NULL;
	bgimg = NULL;
	m_shadow = NULL;
	m_shadowlines = NULL;

	bgpix = new QPixmap(GGZDATADIR "/kdots/dragon.png");
	grayscale(bgpix);
	bgimg = new QImage(bgpix->convertToImage());
	fgimg = new QImage(bgpix->convertToImage());
	setErasePixmap(*bgpix);
}

QDots::~QDots()
{
	if(bgpix) delete bgpix;
	if(bgimg) delete bgimg;
	if(fgimg) delete fgimg;
	if(m_shadow) delete m_shadow;
	if(m_shadowlines) delete m_shadowlines;
}

void QDots::grayscale(QPixmap *pix)
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

	repaint();
}

void QDots::paintEvent(QPaintEvent *e)
{
	int tilewidth, tileheight;
	QRgb rgb;
	int bluepart;
	QPainter p;
	int w, h;
	int ws, hs;
	int updatescreen;

	if((m_rows < 0) || (m_cols < 0)) return;
	if(!m_shadow) return;

	tileheight = height() / (m_aryheight + 2);
	tilewidth = width() / (m_arywidth + 2);

	w = width();
	h = height();
	ws = w / (m_cols + 1);
	hs = h / (m_rows + 1);
	updatescreen = 0;

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

						if(content(i - 1, j - 1) != -1)
						{
							bluepart = qBlue(rgb) / 3;
							if(bluepart < 50) bluepart = 50;
							if(content(i - 1, j - 1) == 1) bluepart *= 4;
							if(bluepart > 255) bluepart = 255;
							rgb = qRgb(0, 0, bluepart);
						}
						fgimg->setPixel(i * tilewidth + x, j * tileheight + y, rgb);
					}
				p.begin(bgpix);
				p.drawImage(i * tilewidth, j * tileheight, *fgimg, i * tilewidth, j * tileheight, tilewidth, tileheight);
				p.end();
				updatescreen = 1;

				m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2) = -2;
				m_shadowlines->at(((j) * m_cols + (i - 1)) * 2) = -2;
				m_shadowlines->at(((j - 1) * m_cols + (i)) * 2) = -2;
				m_shadowlines->at(((j) * m_cols + (i)) * 2) = -2;
				m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2 + 1) = -2;
				m_shadowlines->at(((j) * m_cols + (i - 1)) * 2 + 1) = -2;
				m_shadowlines->at(((j - 1) * m_cols + (i)) * 2 + 1) = -2;
				m_shadowlines->at(((j) * m_cols + (i)) * 2 + 1) = -2;
			}
		}
	}

	p.begin(bgpix);
	for(int j = 1; j <= m_rows; j++)
	{
		for(int i = 1; i <= m_cols; i++)
		{
			if((i < m_cols) && (m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2) != border(i - 1, j - 1, right)))
			{
				m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2) = border(i - 1, j - 1, right);
				if(border(i - 1, j - 1, right) != -1) p.setPen(Qt::darkBlue);
				else p.setPen(Qt::yellow);
				p.drawLine(i * ws, j * hs, (i + 1) * ws, j * hs);
				p.drawLine(i * ws, j * hs + 1, (i + 1) * ws, j * hs + 1);
				updatescreen = 1;
			}

			if((j < m_rows) && (m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2 + 1) != border(i - 1, j - 1, down)))
			{
				m_shadowlines->at(((j - 1) * m_cols + (i - 1)) * 2 + 1) = border(i - 1, j - 1, down);
				if(border(i - 1, j - 1, down) != -1) p.setPen(Qt::darkBlue);
				else p.setPen(Qt::yellow);
				p.drawLine(i * ws, j * hs, i * ws, (j + 1) * hs);
				p.drawLine(i * ws + 1, j * hs, i * ws + 1, (j + 1) * hs);
				updatescreen = 1;
			}

			if(updatescreen)
			{
				p.setPen(QPen(Qt::white));
				p.drawPoint(i * ws, j * hs);
				p.drawPoint(i * ws + 1, j * hs);
				p.drawPoint(i * ws, j * hs + 1);
				p.drawPoint(i * ws + 1, j * hs + 1);
			}
		}
	}
	p.end();

	if(updatescreen) setErasePixmap(*bgpix);
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
	success = vSetBorderValue(x, y, Dots::move);
	if(success) emit signalTurn(m_lastx, m_lasty, m_lastdirection);
}

// Reinitializes shadow structure
void QDots::refreshBoard()
{
	QPainter p;

	if(m_shadow) delete m_shadow;
	if(m_shadowlines) delete m_shadowlines;

	m_shadow = new QMemArray<int>((m_cols - 1) * (m_rows - 1));
	m_shadowlines = new QMemArray<int>(m_cols * m_rows * 2);
	for(int j = 0; j < m_rows - 1; j++)
		for(int i = 0; i < m_cols - 1; i++)
			m_shadow->at(j * (m_cols - 1) + i) = -1;
	for(int j = 0; j < m_rows; j++)
		for(int i = 0; i < m_cols; i++)
		{
			m_shadowlines->at((j * m_cols + i) * 2) = 0;
			m_shadowlines->at((j * m_cols + i) * 2 + 1) = 0;
		}
	m_arywidth = m_cols - 1;
	m_aryheight = m_rows - 1;

	p.begin(bgpix);
	p.drawImage(0, 0, *bgimg, 0, 0, width(), height());
	p.end();
	if(fgimg) delete fgimg;
	fgimg = new QImage(bgpix->convertToImage());
	setErasePixmap(*bgpix);

	resizeEvent(NULL);
	repaint();
}

