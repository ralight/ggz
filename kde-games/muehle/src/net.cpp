// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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
#include "net.h"

// System includes
#include <iostream>
#include <string>
#include <unistd.h>
#include <qstringlist.h>
#include <fcntl.h>

using namespace std;

// Constructor
Net::Net()
: QObject()
{
	fcntl(0, F_SETFL, O_NONBLOCK);
}

// Destructor
Net::~Net()
{
}

// Send a line over the net
void Net::output(const QString &s)
{
	//cout << "[" << s.latin1() << "]" << endl;
	cout << s.latin1() << endl;
}

// Read a single line
QString Net::input()
{
	//string s;
	//cin >> s;
	//return QString(s.c_str());
	return m_input;
}

// Check for network input
void Net::poll()
{
	char tmp[256];
	int ret;

	ret = read(0, tmp, sizeof(tmp));
	if(ret > 0)
	{
		tmp[ret - 1] = 0;
		QString x = tmp;
		QStringList l;
		l = l.split('\n', x);
		for(QStringList::Iterator it = l.begin(); it != l.end(); it++)
		{
			m_input = (*it);
			cerr << "==> " << (*it) << endl;
			emit signalInput();
		}
	}
}

