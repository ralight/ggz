#include "kpfgallery.h"

#include <qlayout.h>

#include <kdebug.h>

KPlayerFaceGallery::KPlayerFaceGallery(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_layout = new QGridLayout(this);
	m_lay = leftrow | rightrow | toprow | bottomrow | edges;

	show();
}

KPlayerFaceGallery::~KPlayerFaceGallery()
{
}

void KPlayerFaceGallery::setMainWidget(QWidget *w)
{
	int x, y;

	x = 5;
	y = 5;

	m_layout->expand(x, y);
	m_layout->addMultiCellWidget(w, 1, y - 2, 1, x - 2);
}

void KPlayerFaceGallery::add(QWidget *w)
{
	int x = -1, y = -1;

	// layout: left/right row, top/bottom row, edges
	for(int j = 0; j < m_layout->numRows(); j++)
	{
		for(int i = 0; i < m_layout->numCols(); i++)
		{
			// rule for intereur
			if((i) && (j) && (i < m_layout->numCols() - 1) && (j > m_layout->numRows() - 1))
				continue;

			// rule for edges
			if(!(m_lay & edges))
				if(((!i) || (i == m_layout->numCols() - 1)) && ((!j) || (j == m_layout->numRows() - 1)))
					continue;

			// rules for sides
			if(((!i) && (m_lay & leftrow)) || ((i == m_layout->numCols() - 1) && (m_lay & leftrow))
			|| ((!j) && (m_lay & toprow)) || ((j == m_layout->numRows() - 1) && (m_lay & bottomrow)))
			{
				if(!m_map[j * m_layout->numRows() + i])
				{
					x = i;
					y = j;
				}
			}
		}
	}

	// apply position
	if((x != -1) && (y != -1))
	{
		m_map[y * m_layout->numRows() + x] = w;
		m_layout->addWidget(w, y, x);

		kdDebug() << "add widget: " << x << ", " << y << endl;
	}
}

void KPlayerFaceGallery::setLayout(int layout)
{
	m_lay = layout;
}

