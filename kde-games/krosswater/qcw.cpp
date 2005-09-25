// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "qcw.h"

// Qt includes
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>

// System includes
#include <cstdlib>
#include <ctime>
#include <iostream>

// Configuration includes
#include "config.h"

using namespace std;

QCw::QCw(QWidget* parent, const char* name)
: QWidget(parent, name)
{
	setEraseColor(Qt::black);
	m_width = -1;
	m_height = -1;
	m_board = NULL;
	m_x = -1;
	m_y = -1;
	m_state = normal;
	m_pix = NULL;
	startTimer(500);
	m_update = 0;
	m_enabled = 0;
	m_turn = -1;
	m_allupdate = 0;

	resetPlayers();
}

QCw::~QCw()
{
}

void QCw::resetPlayers()
{
	m_numplayers = 0;
	for(int i = 0; i < 4; i++)
		m_players[i][2] = -1;
}

void QCw::paintEvent(QPaintEvent *e)
{
	QPainter p;
	char *playerpixmap;
	int dark;

	Q_UNUSED(e);

	if(!m_update) return;
	if(!m_allupdate) return;
	if((m_width < 0) || (m_height < 0)) return;
	m_update = 0;

	if(!m_pix)
	{
		m_pix = new QPixmap(width(), height());
		m_pix->fill(QColor(0, 0, 150));
	}
	p.begin(m_pix);

	p.setPen(QColor(0, 0, 200));
	for(int j = 0; j < m_height; j++)
		for(int i = 0; i < m_width; i++)
		{
			dark = (!i) * 40;
			switch(m_board[i][j][0])
			{
				case inactive:
					p.fillRect(i * 20, j * 20, 20, 20, QBrush(QColor(0, 0, 80)));
					for(int n = 0; n < 10; n++)
						p.drawPoint(rand() % 20 + i * 20, rand() % 20 + j * 20);
					break;
				case active:
				case active2:
					p.fillRect(i * 20 + 2, j * 20 + 2, 16, 16, QBrush(QColor(180 - dark, 110 - dark, 30)));
					break;
				case path:
					p.fillRect(i * 20 + 3, j * 20 + 3, 14, 14, QBrush(QColor(200, 200, 0)));
					break;
				default:
					break;
			}

			switch(m_board[i][j][1])
			{
				case fromframe:
					p.fillRect(i * 20 + 5, j * 20 + 5, 10, 10, QBrush(QColor(200, 200, 0)));
					break;
				case toframe:
					p.fillRect(i * 20 + 5, j * 20 + 5, 10, 10, QBrush(QColor(250, 250, 0)));
					break;
				default:
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
		if(m_players[i][2] >= 0)
		{
			if(m_turn == i) p.fillRect(m_players[i][0] * 20 + 2, m_players[i][1] * 20 + 2, 16, 16, QBrush(QColor(220, 160, 90)));
			p.drawPixmap(m_players[i][0] * 20, m_players[i][1] * 20, QPixmap(playerpixmap));
		}
	}

	if(m_state == selected)
	{
		p.setPen(QColor(255, 255, 0));
		p.drawRect(m_x * 20, m_y * 20, 20, 20);
	}
	else
	{
		if((m_x >= 0) && (m_y >= 0))
		{
			p.setPen(QColor(0, 0, 255));
			p.drawRect(m_x * 20, m_y * 20, 20, 20);
			m_x = -1;
			m_y = -1;
		}
	}

	p.end();
	setErasePixmap(*m_pix);
}

void QCw::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	m_allupdate = 0;

	setFixedSize(m_width * 20, m_height * 20);
	setEraseColor(Qt::blue);

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

	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++)
			m_board[i][j][1] = inactive;

	//m_enabled = 1;
}

void QCw::mousePressEvent(QMouseEvent *e)
{
	int x, y;

	x = e->x() / 20;
	y = e->y() / 20;

	if(!m_board) return;
	if(!m_enabled) return;

	if(m_state == normal)
	{
		if(m_board[x][y][0] == active)
		{
			m_x = x;
			m_y = y;
			m_state = selected;
		}
	}
	else
	{
		m_state = normal;
		if(m_board[x][y][0] == inactive)
			emit signalMove(m_x, m_y, x, y);
	}

	m_update = 1;
	repaint();
}

void QCw::timerEvent(QTimerEvent *e)
{
	Q_UNUSED(e);

	m_update = 1;
	repaint();
}

void QCw::setStone(int x, int y, int value)
{
	if((x < 0) || (y < 0) || (x >= m_width) || (y >= m_height)) return;

	m_board[x][y][0] = value;
	m_update = 1;
	if(m_allupdate) repaint();
}

void QCw::setStoneState(int x, int y, int state)
{
	if((x < 0) || (y < 0) || (x >= m_width) || (y >= m_height)) return;

	m_board[x][y][1] = state;
	m_update = 1;
	repaint();
}

void QCw::addPlayer(int x, int y)
{
	if(m_numplayers == 4) return;
	m_players[m_numplayers][0] = x;
	m_players[m_numplayers][1] = y;
	m_numplayers++;
}

void QCw::setPlayerPixmap(int player, int pixmap)
{
	if((player < 0) || (player > m_numplayers)) return;
	m_players[player][2] = pixmap;
}

void QCw::disable()
{
	m_enabled = 0;
}

void QCw::enable()
{
	m_allupdate = 1;
	m_enabled = 1;
	repaint();
}

void QCw::setPlayerTurn(int player)
{
	m_turn = player;
}

