#include "qcw.h"

#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>

#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "config.h"

QCw::QCw(QWidget* parent = 0, char* name = 0)
: QWidget(parent, name)
{
	setBackgroundColor(Qt::black);
	m_width = -1;
	m_height = -1;
	m_board = NULL;
	m_x = -1;
	m_y = -1;
	m_state = normal;
	startTimer(1000);

	resetPlayers();
}

QCw::~QCw()
{
}

void QCw::resetPlayers()
{
	cout << "QCW:: Remove all players" << endl;
	m_numplayers = 0;
	for(int i = 0; i < 4; i++)
		m_players[i][2] = -1;
}

void QCw::paintEvent(QPaintEvent *e)
{
	QPainter p;
	char *playerpixmap;

	if((m_width < 0) || (m_height < 0)) return;

	p.begin(this);

	p.setPen(QColor(0.0, 0.0, 255.0));
	for(int j = 0; j < m_height; j++)
		for(int i = 0; i < m_width; i++)
		{
			switch(m_board[i][j][0])
			{
				case 0:
					p.fillRect(i * 20, j * 20, 20, 20, QBrush(QColor(0.0, 0.0, 160.0)));
					for(int n = 0; n < 10; n++)
						p.drawPoint(rand() % 20 + i * 20, rand() % 20 + j * 20);
					break;
				case 1:
				case 2:
					p.fillRect(i * 20 + 2, j * 20 + 2, 16, 16, QBrush(QColor(180.0, 70.0, 0.0)));
					break;
				case 3:
					p.fillRect(i * 20 + 3, j * 20 + 3, 14, 14, QBrush(QColor(200.0, 200.0, 0.0)));
					break;
				default:
					cout << "huh? " << m_board[i][j][0] << endl;
			}

			switch(m_board[i][j][1])
			{
				case -1:
					p.fillRect(i * 20 + 5, j * 20 + 5, 10, 10, QBrush(QColor(200.0, 200.0, 0.0)));
					break;
				case -2:
					p.fillRect(i * 20 + 5, j * 20 + 5, 10, 10, QBrush(QColor(250.0, 250.0, 0.0)));
					break;
			}
		}

	for(int i = 0; i < m_numplayers; i++)
	{
		switch(m_players[i][2])
		{
			case 0:
				playerpixmap = GGZDATADIR "/krosswater/gfx/franzl.png";
				break;
			case 1:
				playerpixmap = GGZDATADIR "/krosswater/gfx/alien.png";
				break;
			case 2:
			default:
				playerpixmap = GGZDATADIR "/krosswater/gfx/geek.png";
				break;
		}
		if(m_players[i][2] >= 0) p.drawPixmap(m_players[i][0] * 20, m_players[i][1] * 20, QPixmap(playerpixmap));
	}

	if(m_state == selected)
	{
		p.setPen(QColor(255.0, 255.0, 0.0));
		p.drawRect(m_x * 20, m_y * 20, 20, 20);
	}
	else
	{
		if((m_x >= 0) && (m_y >= 0))
		{
			p.setPen(QColor(0.0, 0.0, 255.0));
			p.drawRect(m_x * 20, m_y * 20, 20, 20);
			m_x = -1;
			m_y = -1;
		}
	}

	p.end();
}

void QCw::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	setFixedSize(m_width * 20, m_height * 20);
	setBackgroundColor(Qt::blue);

	if(m_board)
	{
		for(int i = 0; i < m_width; i++)
		{
			for(int j = 0; j < m_height; j++)
				free(m_board[i][j]);
			free(m_board[i]);
		}
		free(m_board);
	}

	m_board = (int***)malloc(width * sizeof(int));
	for(int i = 0; i < width; i++)
	{
		m_board[i] = (int**)malloc(height * sizeof(int));
		for(int j = 0; j < height; j++)
			m_board[i][j] = (int*)malloc(2);
	}

	/*srandom(time(NULL));*/
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++)
			m_board[i][j][1] = 0;
}

void QCw::mousePressEvent(QMouseEvent *e)
{
	int x, y;

	x = e->x() / 20;
	y = e->y() / 20;
	cout << "Clicked on " << x << ", " << y << endl;

	if(!m_board) return;

	if(m_state == normal)
	{
		if(m_board[x][y][0] == 1)
		{
			m_x = x;
			m_y = y;
			m_state = selected;
		}
	}
	else
	{
		m_state = normal;
		if(m_board[x][y][0] == 0)
			emit signalMove(m_x, m_y, x, y);
	}

	paintEvent(NULL);
}

void QCw::timerEvent(QTimerEvent *e)
{
	paintEvent(NULL);
}

void QCw::setStone(int x, int y, int value)
{
	if((x < 0) || (y < 0) || (x >= m_width) || (y >= m_height)) return;

	m_board[x][y][0] = value;
}

void QCw::setStoneState(int x, int y, int state)
{
	if((x < 0) || (y < 0) || (x >= m_width) || (y >= m_height)) return;

	m_board[x][y][1] = state;
}

void QCw::addPlayer(int x, int y)
{
	if(m_numplayers == 4) return;
	m_players[m_numplayers][0] = x;
	m_players[m_numplayers][1] = y;
	m_numplayers++;
	cout << "QCW: setPlayer at " << x << ", " << y << endl;
}

void QCw::setPlayerPixmap(int player, int pixmap)
{
	if((player < 0) || (player > m_numplayers)) return;
	m_players[player][2] = pixmap;
	cout << "QCW: setPlayerPixmap for " << player << endl;
}

