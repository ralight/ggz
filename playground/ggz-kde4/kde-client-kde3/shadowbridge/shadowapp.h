// ShadowBridge - Game developer tool to visualize network protocols
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

#ifndef SHADOW_APP_H
#define SHADOW_APP_H

// KDE includes
#include <kmainwindow.h>

// Forward declarations
class ShadowContainer;
class ShadowClient;
class ShadowClientGGZ;
class ShadowNet;

// Main shadowbridge window
class ShadowApp : public KMainWindow
{
	Q_OBJECT
	public:
		ShadowApp();
		~ShadowApp();

		enum MenuItems
		{
			menuadd,
			menuaddggz,
			menustart,
			menuquit,
			menudebugin,
			menudebugout,
			menudebugsync
		};

	public slots:
		void slotMenu(int id);
		void slotClient(const QString& exec, const QString& fdin, const QString& fdout);
		void slotError(const QString& error);

	private:
		ShadowContainer *container;
		ShadowClient *client;
		ShadowClientGGZ *clientggz;
		ShadowNet *net;
};

#endif

