#include "qnetmapsample.h"

#include <qpainter.h>
#include <qlabel.h>
#include <qfont.h>

#include <NetmapResourceFileText>

#define SPRITEDIR "sprites"
#define SPRITEWIDTH 64
#define SPRITEHEIGHT 64
#define BORDERSIZE 2

#include <iostream>

int xoffset, yoffset;
QLabel *bubble;

QNetmapSample::QNetmapSample(QWidget *parent, char *name)
: QNetmap(parent, name)
{
	NetmapResourceFileText *resource;

	m_map = new NetmapMap();
	setMap(m_map);

	resource = new NetmapResourceFileText();
	resource->openFile("/tmp/random.map");

	m_map->setResource(resource);

	m_sprite_counter = 1;

	pushSprite(SPRITEDIR "/dark.png");
	pushSprite(SPRITEDIR "/dleft.png");
	pushSprite(SPRITEDIR "/dright.png");
	pushSprite(SPRITEDIR "/ddown.png");
	pushSprite(SPRITEDIR "/dup.png");
	pushSprite(SPRITEDIR "/dlu.png");
	pushSprite(SPRITEDIR "/dld.png");
	pushSprite(SPRITEDIR "/dru.png");
	pushSprite(SPRITEDIR "/drd.png");
	pushSprite(SPRITEDIR "/grass.png");

	pushSprite(SPRITEDIR "/boy.png");

	m_map->setPosition(0, 0);
	m_map->update();

	xoffset = 0;
	yoffset = 0;

	bubble = new QLabel("", this);
	bubble->setFixedSize(120, 15);
	bubble->setBackgroundColor(QColor(255.0, 255.0, 255.0));
	QFont font("lucida");
	font.setPointSize(10);
	bubble->setFont(font);
	bubble->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	bubble->setIndent(0);
	bubble->hide();
}

QNetmapSample::~QNetmapSample()
{
}

void QNetmapSample::keyPressEvent(QKeyEvent *e)
{
	int ignore;
	static int bubblemode = 0;

	cout << "QNetmapSample: Pressed key: " << e->ascii() << endl;

	if(e->ascii() == 27) exit(0);
	ignore = 1;

	/*begin quick*/
	if(e->ascii() == 13)
	{
		bubblemode = !bubblemode;
		if(bubblemode)
		{
			cout << "bubble show" << endl;
			bubble->move((m_map->x() - xoffset) * SPRITEWIDTH - bubble->width() / 2, (m_map->y() - yoffset) * SPRITEHEIGHT - 20);
			bubble->show();
			//bubble->setText("hello world");
			cout << "length: " << bubble->text().length() << endl;
			//bubble->text().truncate(bubble->text().length() - 1);
		}
		else
		{
			cout << "bubble hide" << endl;
			bubble->hide();
			bubble->clear();
			drawMap();
		}
	}
	else
	{
		if(bubblemode)
		{
			bubble->setText(bubble->text() + e->text());
			cout << "yeah;): " << bubble->text() << endl;
			return;
		}
	}
	/*end quick*/

	switch(e->key())
	{
		case Qt::Key_Left:
			if(!bubblemode)
			{
				m_map->setPosition(m_map->x() - 1, m_map->y());
				ignore = 0;
			}
			break;
		case Qt::Key_Right:
			if(!bubblemode)
			{
				m_map->setPosition(m_map->x() + 1, m_map->y());
				ignore = 0;
			}
			break;
		case Qt::Key_Up:
			if(!bubblemode)
			{
				m_map->setPosition(m_map->x(), m_map->y() - 1);
				ignore = 0;
			}
			break;
		case Qt::Key_Down:
			if(!bubblemode)
			{
				m_map->setPosition(m_map->x(), m_map->y() + 1);
				ignore = 0;
			}
			break;
		case Qt::Key_Enter:
			bubblemode = !bubblemode;
			if(bubblemode)
			{
				cout << "bubble show" << endl;
				bubble->show();
			}
			else
			{
				cout << "bubble hide" << endl;
				bubble->hide();
			}
			break;
		//default:
			//if(bubblemode)
			//{
				//bubble->setText(bubble->text() + e->text());
			//}
	}

	if(!ignore)
	{
		cout << "Pos is now: " << m_map->x() << "/" << m_map->y() << endl;
		drawMap();
	}
}

void QNetmapSample::pushSprite(char *spritename)
{
	setSprite(m_sprite_counter, QPixmap(spritename));
	m_sprite_counter++;
}

void QNetmapSample::drawTile(int x, int y, int num)
{
	QPainter painter;
	QPixmap pixmap;
	int spritenum;

	// Log this as an error?
	if(!num) return;

	//spritenum = num % (m_sprite_counter - 2) + 1;
	//if(spritenum < 0) spritenum = -spritenum;
	spritenum = num;

	cout << "fetch pixmap. no " << spritenum << endl;
	pixmap = sprite(spritenum);

	cout << "  draw tile at " << x * SPRITEWIDTH << ", " << y * SPRITEHEIGHT << endl;
	painter.begin(this);
	painter.drawPixmap(x * SPRITEWIDTH, y * SPRITEHEIGHT, pixmap);
	painter.end();
}

void QNetmapSample::drawMap()
{
	Ntile *tile;

	// calculate offsets
	if(m_map->y() - yoffset > m_sprites_vertical - BORDERSIZE) yoffset = m_map->y() - m_sprites_vertical + BORDERSIZE;
	if(m_map->x() - xoffset > m_sprites_horizontal - BORDERSIZE) xoffset = m_map->x() - m_sprites_horizontal + BORDERSIZE;
	if(m_map->y() - yoffset < BORDERSIZE - 1) yoffset = m_map->y() - BORDERSIZE + 1;
	if(m_map->x() - xoffset < BORDERSIZE - 1) xoffset = m_map->x() - BORDERSIZE + 1;
	if(yoffset < 0) yoffset = 0;
	if(xoffset < 0) xoffset = 0;

	cout << "draw map." << endl;
	erase();

	for(int j = m_map->y() - 2; j <= m_map->y() + 2; j++)
		for(int i = m_map->x() - 2; i <= m_map->x() + 2; i++)
		{
			tile = m_map->tile(i, j);
			if(tile) drawTile(i - xoffset, j - yoffset, *tile);
		}
	drawTile(m_map->x() - xoffset, m_map->y() - yoffset, 11);
}

void QNetmapSample::resizeEvent(QResizeEvent *e)
{
	m_sprites_horizontal = e->size().width() / SPRITEWIDTH + 1;
	m_sprites_vertical = e->size().height() / SPRITEHEIGHT + 1;

	cout << "Resizing to " << e->size().width() << " x " << e->size().height() << endl;
	cout << "(That is: " << m_sprites_horizontal << " x " << m_sprites_vertical << ")" << endl;
}
