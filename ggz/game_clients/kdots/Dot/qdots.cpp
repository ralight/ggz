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

#include <qpainter.h>
#include <qpen.h>

#include <iostream>

QDots::QDots(QWidget* parent = 0, char* name = 0)
: QWidget(parent, name), VDots()
{
	setBackgroundColor(Qt::blue);
}

QDots::~QDots()
{
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

	cout << "RESIZED(0): " << w << ", " << h << endl;

	vResizeBoard(w, h);

	paintEvent(NULL);
}

void QDots::paintEvent(QPaintEvent *e)
{
	QPainter p;
	int w, h;
	int ws, hs;

	if((m_rows < 0) || (m_cols < 0)) return;

	w = width();
	h = height();
	ws = w / (m_cols + 1);
	hs = h / (m_rows + 1);
	//ws < hs ? hs = ws : ws = hs;

	p.begin(this);

	for(int j = 1; j <= m_rows; j++)
		for(int i = 1; i <= m_cols; i++)
		{
			if(i < m_cols)
			{
				if(border(i - 1, j - 1, right) == 1) p.setPen(Qt::white);
				else p.setPen(Qt::darkBlue);
				p.drawLine(i * ws, j * hs, (i + 1) * ws, j * hs);
			}

			if(j < m_rows)
			{
				if(border(i - 1, j - 1, down) == 1) p.setPen(Qt::white);
				else p.setPen(Qt::darkBlue);
				p.drawLine(i * ws, j * hs, i * ws, (j + 1) * hs);
			}

			if((i < m_cols) && (j < m_rows) && (borders(i - 1, j - 1, 1) == 4))
				p.fillRect(i * ws + 2, j * hs + 2, ws - 3, hs - 3, QBrush(QColor(255.0, 255.0, 0.0)));

			p.setPen(QPen(Qt::white, 4));
			p.drawPoint(i * ws, j * hs);
		}

	p.end();
}

void QDots::mousePressEvent(QMouseEvent *e)
{
	int x, y;
	int success;

	x = e->x();
	y = e->y();

	cout << "clicked at " << x << ", " << y << endl;

	x -= m_xoffset;
	y -= m_yoffset;
	cout << "Offset: " << m_xoffset << " " << m_yoffset << endl;
	cout << "recalc(0) at " << x << ", " << y << endl;

	//x = (x * (m_cols + 2)) / m_cols;
	//y = (y * (m_rows + 2)) / m_rows;
	//cout << "recalc(1) at " << x << ", " << y << endl;

	success = vSetBorderValue(x, y, 1);
	if(success) emit signalTurn(m_lastx, m_lasty, m_lastdirection);

	paintEvent(NULL);
}

void QDots::refreshBoard()
{
	cout << "refresh" << endl;
	resizeEvent(NULL);
	paintEvent(NULL);
}
