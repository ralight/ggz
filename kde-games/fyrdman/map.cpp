#include "map.h"

#include <kdebug.h>

#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>

#include <stdlib.h>

Map::Map(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	setBackgroundPixmap(QPixmap("data/bayeux.png"));

	setupMap(6, 13);
}

Map::~Map()
{
}

void Map::paintEvent(QPaintEvent *e)
{
}

void Map::mousePressEvent(QMouseEvent *e)
{
	const int fwidth = 60;
	const int fheight = 60;
	const int offx = 50;
	const int offy = 50;
	const int angle = 20;
	int roffx;
	const int xs = fwidth * 2 - angle * 2;
	const int ys = fheight - (int)(angle * 1.5);
	int xpos, ypos;

	if(e->button() == LeftButton)
	{
		for(int j = 0; j < m_height; j++)
			for(int i = 0; i < m_width; i++)
			{
				roffx = offx;
				if(j % 2) roffx = offx + fwidth - angle;

				xpos = i * xs + roffx;
				ypos = j * ys + offy;

				if(((abs(e->x() - (xpos + fwidth / 2)) < 12))
				&& ((abs(e->y() - (ypos + fheight / 2)) < 12)))
				{
					kdDebug() << j << " - " << i << endl;
				}
			}
	}
}

void Map::setupMap(int x, int y)
{
	QPainter p;
	const int fwidth = 60;
	const int fheight = 60;
	const int offx = 50;
	const int offy = 50;
	const int angle = 20;
	int roffx;
	const int xs = fwidth * 2 - angle * 2;
	const int ys = fheight - (int)(angle * 1.5);
	int xpos, ypos;
	const QPixmap *pix;
	QPixmap pix2, pix3;

	m_width = x;
	m_height = y;

	pix = backgroundPixmap();
	QImage im = pix->convertToImage();
	QImage im2 = im.smoothScale((m_width + 1) * xs + offx * 2, (m_height + 1) * ys + offy * 2);
	pix2.convertFromImage(im2);

	pix3.resize((m_width + 1) * xs + offx * 2, (m_height + 1) * ys + offy * 2);
	pix3.fill(QColor(0, 0, 0));

	// Paint hex fields

	p.begin(&pix3);
	p.setBrush(QColor(255, 255, 255));
	p.setPen(QPen(QColor(255, 255, 255), 2, DotLine));

	for(int j = 0; j < m_height; j++)
		for(int i = 0; i < m_width; i++)
		{
			roffx = offx;
			if(j % 2) roffx = offx + fwidth - angle;

			xpos = i * xs + roffx;
			ypos = j * ys + offy;

			int x = rand() % 4;
			if(x > 2) x = 0;
			p.setBrush(QColor(x, x, x));

			QPointArray a(6);
			a.putPoints(0, 6,
				xpos + angle, ypos,
				xpos + fwidth - angle, ypos,
				xpos + fwidth, ypos + fheight / 2,
				xpos + fwidth - angle, ypos + fheight,
				xpos + angle, ypos + fheight,
				xpos, ypos + fheight / 2);

			p.drawPolygon(a);
		}

	p.end();

	setFixedSize((m_width + 1) * xs + offx * 2, (m_height + 1) * ys + offy * 2);

	// Paint possessions

	QImage xim1 = pix3.convertToImage();
	QImage xim2 = pix2.convertToImage();
	for(int j = 0; j < (m_height + 1) * ys + offy * 2; j++)
		for(int i = 0; i < (m_width + 1) * xs + offx * 2; i++)
		{
			int r, g, b, a;
			QRgb tripel, tripel2;

			tripel = xim1.pixel(i, j);
			if(qRed(tripel))
			{
				tripel2 = xim2.pixel(i, j);
				if(qRed(tripel) == 1)
				{
					r = qRed(tripel2) + 40;
					g = qGreen(tripel2) - 40;
					b = qBlue(tripel2) - 40;
				}
				else if(qRed(tripel) == 2)
				{
					r = qRed(tripel2) - 40;
					g = qGreen(tripel2) - 40;
					b = qBlue(tripel2) + 40;
				}
				if(r < 0) r = 0;
				if(g < 0) g = 0;
				if(b < 0) b = 0;
				xim2.setPixel(i, j, qRgb(r, g, b));
			}
		}
	pix2.convertFromImage(xim2);

	// Paint stippled borders

	p.begin(&pix2);
	p.setBrush(QColor(255, 255, 255));
	p.setPen(QPen(QColor(255, 255, 255), 2, DotLine));

	for(int j = 0; j < m_height; j++)
		for(int i = 0; i < m_width; i++)
		{
			roffx = offx;
			if(j % 2) roffx = offx + fwidth - angle;

			xpos = i * xs + roffx;
			ypos = j * ys + offy;

			p.drawLine(xpos + angle, ypos, xpos + fwidth - angle, ypos);
			p.drawLine(xpos + angle, ypos + fheight, xpos + fwidth - angle, ypos + fheight);

			p.drawLine(xpos + angle, ypos, xpos, ypos + fheight / 2);
			p.drawLine(xpos + angle, ypos + fheight, xpos, ypos + fheight / 2);

			p.drawLine(xpos + fwidth - angle, ypos, xpos + fwidth, ypos + fheight / 2);
			p.drawLine(xpos + fwidth - angle, ypos + fheight, xpos + fwidth, ypos + fheight / 2);

		}

	p.end();

	// Paint units

	p.begin(&pix2);

	for(int j = 0; j < m_height; j++)
		for(int i = 0; i < m_width; i++)
		{
			roffx = offx;
			if(j % 2) roffx = offx + fwidth - angle;

			xpos = i * xs + roffx;
			ypos = j * ys + offy;

			int x = rand() % 10;
			if(x == 1)
				p.drawPixmap(QPoint(xpos + angle, ypos - angle), QPixmap("data/knight2.png"));
			else if(x == 2)
				p.drawPixmap(QPoint(xpos + angle, ypos - angle), QPixmap("data/knight1.png"));
		}

	p.end();

	setBackgroundPixmap(pix2);
}

