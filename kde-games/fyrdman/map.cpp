#include "map.h"
#include "level.h"

#include <kdebug.h>

#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>

#include <stdlib.h>

#include "config.h"

Map::Map(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_level = NULL;

	m_map = true;
	m_knights = true;
	m_possession = true;
	m_animation = true;

	m_picked = false;

	setupMap(NULL);
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
	const int fheight = /*60*/50;
	const int offx = 50;
	const int offy = 20/*50*/;
	const int angle = 20;
	int roffx;
	const int xs = fwidth * 2 - angle * 2;
	const int ys = fheight / 2; // == fheight - (int)(angle * /*1.5*/1.25);
	int xpos, ypos;

	if(e->button() == LeftButton)
	{
		for(int j = 0; j < m_height; j++)
			for(int i = 0; i < m_width; i++)
			{
				roffx = offx;
				if(j % 2) roffx = offx + fwidth - angle - xs;

				xpos = i * xs + roffx;
				ypos = j * ys + offy;

				if(((abs(e->x() - (xpos + fwidth / 2)) < 12))
				&& ((abs(e->y() - (ypos + fheight / 2)) < 12)))
				{
					kdDebug() << j << " - " << i << endl;

					if(!m_picked)
					{
						m_x = i;
						m_y = j;
						m_picked = true;
					}
					else
					{
						m_x2 = i;
						m_y2 = j;
						m_picked = false;
						if((m_x != m_x2) || (m_y != m_y2))
						{
							emit signalMove(m_x, m_y, m_x2, m_y2);
						}
					}
				}
			}
	}
}

void Map::setupMap(Level *level)
{
	QPainter p;
	const int fwidth = 60;
	const int fheight = /*60*/50;
	const int offx = 50;
	const int offy = 20/*50*/;
	const int angle = 20;
	int roffx, roffy;
	const int xs = fwidth * 2 - angle * 2;
	const int ys = fheight / 2; // == fheight - (int)(angle * /*1.5*/1.25);
	//const int xs = fwidth - (int)(angle * 1.5);
	//const int ys = fheight * 2 - angle * 2;
	int xpos, ypos;
	const QPixmap *pix;
	QPixmap pix2, pix3;

	setBackgroundPixmap(QPixmap(GGZDATADIR "/fyrdman/bayeux.png"));

	m_level = level;
	if(!level)
	{
		setFixedSize(400, 300);
		return;
	}

	m_width = level->width();
	m_height = level->height();

	pix = backgroundPixmap();
	QImage im = pix->convertToImage();
	QImage im2 = im.smoothScale((m_width + 1) * xs - offx /*+ offx * 2*/, (m_height + 1) * ys + offy /** 2*/);
	pix2.convertFromImage(im2);

	pix3.resize((m_width + 1) * xs - offx /*+ offx * 2*/, (m_height + 1) * ys + offy /** 2*/);
	pix3.fill(QColor(0, 0, 0));

	// Paint hex fields

	if(m_possession)
	{
		p.begin(&pix3);
		p.setBrush(QColor(255, 255, 255));
		p.setPen(QPen(QColor(255, 255, 255), 2, DotLine));

		for(int j = 0; j < m_height; j++)
			for(int i = 0; i < m_width; i++)
			{
				roffx = offx;
				if(j % 2) roffx = offx + fwidth - angle - xs;
				roffy = offy;
				//if(i % 2) roffy = offy + fheight - angle;

				xpos = i * xs + roffx;
				ypos = j * ys + roffy;

				int x = level->cellown(i, j);
				if(x >= 0)
				{
					p.setBrush(QColor(x % 2 + 1, x % 2 + 1, x % 2 + 1));

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
			}

		p.end();
	}

	setFixedSize((m_width + 1) * xs - offx /*+ offx * 2*/, (m_height + 1) * ys + offy /** 2*/);

	// Paint possessions

	if(m_possession)
	{
		QImage xim1 = pix3.convertToImage();
		QImage xim2 = pix2.convertToImage();
		for(int j = 0; j < (m_height + 1) * ys + offy /** 2*/; j++)
			for(int i = 0; i < (m_width + 1) * xs - offx /*+ offx * 2*/; i++)
			{
				int r = 0, g = 0, b = 0;
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
	}

	// Paint stippled borders

	if(m_map)
	{
		p.begin(&pix2);
		p.setBrush(QColor(255, 255, 255));
		p.setPen(QPen(QColor(255, 255, 255), 2, DotLine));

		for(int j = 0; j < m_height; j++)
			for(int i = 0; i < m_width; i++)
			{
				roffx = offx;
				if(j % 2) roffx = offx + fwidth - angle - xs;
				roffy = offy;
				//if(i % 2) roffy = offy + fheight - angle;

				xpos = i * xs + roffx;
				ypos = j * ys + roffy;

				if(level->cellboard(i, j) != 32)
				{
					p.drawLine(xpos + angle, ypos, xpos + fwidth - angle, ypos);
					p.drawLine(xpos + angle, ypos + fheight, xpos + fwidth - angle, ypos + fheight);

					p.drawLine(xpos + angle, ypos, xpos, ypos + fheight / 2);
					p.drawLine(xpos + angle, ypos + fheight, xpos, ypos + fheight / 2);

					p.drawLine(xpos + fwidth - angle, ypos, xpos + fwidth, ypos + fheight / 2);
					p.drawLine(xpos + fwidth - angle, ypos + fheight, xpos + fwidth, ypos + fheight / 2);
				}
			}

		p.end();
	}

	// Paint units

	if(m_knights)
	{
		p.begin(&pix2);

		for(int j = 0; j < m_height; j++)
			for(int i = 0; i < m_width; i++)
			{
				roffx = offx;
				if(j % 2) roffx = offx + fwidth - angle - xs;
				roffy = offy;
				//if(i % 2) roffy = offy + fheight - angle;

				xpos = i * xs + roffx;
				ypos = j * ys + roffy;

				int x = level->cell(i, j);
				if(x >= 0)
				{
					if(x % 2)
						p.drawPixmap(QPoint(xpos + angle, ypos - angle / 2), QPixmap(GGZDATADIR "/fyrdman/knight2.png"));
					else
						p.drawPixmap(QPoint(xpos + angle, ypos - angle / 2), QPixmap(GGZDATADIR "/fyrdman/knight1.png"));
				}

				if(x >= 0)
				{
					p.fillRect(xpos + angle, ypos + 2 * angle, fwidth - 2 * angle, 7, QBrush(QColor(0, 0, 0)));
					QColor c;
					if(x % 2) c = QColor(0, 0, 255);
					else c = QColor(255, 0, 0);
					p.fillRect(xpos + angle + 1, ypos + 2 * angle + 1, fwidth - 2 * angle - 2, 2, QBrush(c));
					if(x == 0) c = QColor(255, 0, 0);
					else if(x == 1) c = QColor(0, 255, 0);
					else if(x == 2) c = QColor(0, 0, 255);
					else if(x == 3) c = QColor(0, 255, 255);
					else if(x == 4) c = QColor(255, 255, 0);
					else if(x == 5) c = QColor(255, 0, 255);
					else if(x == 6) c = QColor(0, 128, 0);
					else if(x == 7) c = QColor(255, 128, 255);
					else if(x == 8) c = QColor(0, 128, 255);
					else if(x == 9) c = QColor(255, 128, 0);
					else if(x == 10) c = QColor(128, 128, 0);
					else if(x == 11) c = QColor(0, 128, 128);
					p.fillRect(xpos + angle + 1, ypos + 2 * angle + 4, fwidth - 2 * angle - 2, 2, QBrush(c));
				}
			}

		p.end();
	}

	setBackgroundPixmap(pix2);
}

void Map::setMap(bool map)
{
	m_map = map;
	setupMap(m_level);
}

void Map::setKnights(bool knights)
{
	m_knights = knights;
	setupMap(m_level);
}

void Map::setPossession(bool possession)
{
	m_possession = possession;
	setupMap(m_level);
}

void Map::setAnimation(bool animation)
{
	m_animation = animation;
	setupMap(m_level);
}

void Map::move(int x, int y, int x2, int y2)
{
	if(m_level)
	{
		int tmp = m_level->cell(x, y);
		m_level->setCell(x, y, -1);
		m_level->setCell(x2, y2, tmp);
		setupMap(m_level);
	}
}

