// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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
#include "canvas.h"

// Keepalive includes
#include "protocol.h"
#include "config.h"

// Qt includes
#include <qsocketnotifier.h>
#include <qsocketdevice.h>
#include <qdatastream.h>
#include <qmessagebox.h>

// System includes
#include <iostream>
#include <cstdlib>

#define KEEPALIVE_DIR GGZDATADIR "/keepalive"

// Constructor: build empty canvas
Canvas::Canvas(QWidget *parent, const char *name)
: QCanvas(parent, name)
{
	m_dev = NULL;
	m_net = NULL;

	setBackgroundPixmap(QPixmap(KEEPALIVE_DIR "/grass.png"));

	setAdvancePeriod(50);
}

// Destructor
Canvas::~Canvas()
{
}

// Initialize the network
void Canvas::init()
{
	QSocketNotifier *sn;
	int socket;

	socket = 3;
	sn = new QSocketNotifier(socket, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotInput()));
	m_dev = new QSocketDevice(socket, QSocketDevice::Stream);
	m_net = new QDataStream(m_dev);
}

// Receive game data from server
void Canvas::slotInput()
{
	signed char c;
	int width, height;
	int x, y;
	int count;
	char *name, *message;
	QCanvasSprite *sprite;

	*m_net >> c;

	std::cout << "Got input: " << (int)c << std::endl;

	switch(c)
	{
		case op_map:
			*m_net >> width >> height;
			resize(width, height);
			break;
		case op_init:
			*m_net >> x >> y;
			std::cout << "Move to " << x << "/" << y << std::endl;
			m_player = new QCanvasSprite(new QCanvasPixmapArray(KEEPALIVE_DIR "/man.png", 1), this);
			m_player->move(x, y);
			m_player->show();
			break;
		case op_player:
			*m_net >> count;
			std::cout << "Receive data of " << count << " players" << std::endl;
			for(int i = 0; i < count; i++)
			{
				*m_net >> name;
				*m_net >> x >> y;
				std::cout << name << " (" << x << "/" << y << ")" << std::endl;
				sprite = new QCanvasSprite(new QCanvasPixmapArray(KEEPALIVE_DIR "/avatar.png", 1), this);
				sprite->move(x, y);
				sprite->show();
				free(name);
			}
			break;
		case op_name:
			*m_net >> name;
			emit signalLoggedin(name);
			free(name);
			break;
		case op_moved:
			*m_net >> name;
			*m_net >> x >> y;
			std::cout << "Receive move of: " << name << ": " << x << ", " << y << std::endl;
			sprite = new QCanvasSprite(new QCanvasPixmapArray(KEEPALIVE_DIR "/avatar.png", 1), this);
			sprite->move(x, y);
			sprite->show();
			free(name);
			break;
		case op_loginfailed:
			//*m_net >> name;
			//*m_net >> message;
			QMessageBox::information(NULL, "Notice", "Login failed");
			//free(name);
			//free(message);
			break;
		case op_chat:
			QMessageBox::information(NULL, "Chat", QString("Chat from %1:\n%2").arg(name).arg(message));
			break;
		default:
			//QMessageBox::information(NULL, "Notice", QString("Unknown opcode: %1").arg((int)c));
			std::cout << "Unknown opcode: " << (int)c << std::endl;
	}
}

// Move for the given distance if it's valid
void Canvas::move(int x, int y)
{
	int vx, vy;

	if(!m_net) return;

	std::cout << "key-1" << std::endl;
	if(!(x || y)) return;
	std::cout << "key-2" << std::endl;

	vx = (int)(m_player->x() + x * 3);
	vy = (int)(m_player->y() + y * 3);
	if((vx >= 0)
	&& (vy >= 0)
	&& (vx < width() - 32)
	&& (vy < height() - 32))
	{
		m_player->move(vx, vy);
		std::cout << "Moving to " << vx << "/" << vy << std::endl;
		*m_net << (Q_INT8)op_move << (Q_INT32)vx << (Q_INT32)vy;
	}
}

// Log into the game server
void Canvas::login(QString username, QString password)
{
	if(!m_net) init();

	*m_net << (Q_INT8)op_login << username.latin1() << password.latin1();
}

void Canvas::keyPressEvent(QKeyEvent *e)
{
	std::cout << "extra key press event!" << std::endl;
}

void Canvas::chat(QString message)
{
	*m_net << (Q_INT8)op_chat << message.latin1();
}

