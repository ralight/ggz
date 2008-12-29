//
// GGZ Starterpack for C++ - Sample Client
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#ifndef SAMPLE_GAMEWIN_H
#define SAMPLE_GAMEWIN_H

#include "net.h"

#include <kggzmod/module.h>
#include <kmainwindow.h>
#include <qmap.h>
#include <qlist.h>
#include <qdir.h>

class KMenu;
class QAction;

namespace KGGZMod
{
	class Module;
};

class GameWin : public KMainWindow
{
	Q_OBJECT
	public:
		GameWin();
		void enableNetwork(bool enabled);

	private slots:
		void slotStatus(const QString &status);
		void slotError();
		void slotNetwork(int fd);
		void slotPacket(ggz_starterpackOpcodes::Opcode opcode, const msg& message);

	private:
		void connectcore();
		void slotMenu(QAction *action);

		ggz_starterpack *m_proto;
		KGGZMod::Module *m_mod;
		KMenu *mgame;
		bool m_networked;

		QAction *action_connect;
		QAction *action_quit;
		QAction *action_ggzplayers;
};

#endif

