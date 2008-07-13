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

#include "krosswater_move.h"

KrosswaterMove::KrosswaterMove(int fromx, int fromy, int tox, int toy)
{
	m_fromx = fromx;
	m_fromy = fromy;
	m_tox = tox;
	m_toy = toy;
}

KrosswaterMove::~KrosswaterMove()
{
}

int KrosswaterMove::fromx()
{
	return m_fromx;
}

int KrosswaterMove::fromy()
{
	return m_fromy;
}

int KrosswaterMove::tox()
{
	return m_tox;
}

int KrosswaterMove::toy()
{
	return m_toy;
}

